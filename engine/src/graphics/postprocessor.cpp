#include "graphics/postprocessor.hpp"

#include "core/log.hpp"
#include "core/glerror.hpp"

namespace engine::graphics
{
    int PostProcessor::initialize()
    {
        if (!initialize_vao())
        {
            return 0;
        }

        return 1;
    }

    int PostProcessor::initialize_vao()
    {
        // initialize render data
        // configure VAO/VBO
        GLuint vbo;
        float vertices[] = {
            // pos        // tex
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,

            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f};

        glGenVertexArrays(1, &_vao);
        glCheckError();
        glGenBuffers(1, &vbo);
        glCheckError();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glCheckError();
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glCheckError();

        glBindVertexArray(_vao);
        glCheckError();
        glEnableVertexAttribArray(0);
        glCheckError();
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glCheckError();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glCheckError();
        glBindVertexArray(0);
        glCheckError();

        return 1;
    }

    void PostProcessor::begin()
    {
        const auto first_effect = _effects.begin();
        if (first_effect == _effects.end())
        {
            return;
        }
        first_effect->activate_render_target();
    }

    void PostProcessor::end()
    {
        const auto first_effect = _effects.begin();
        if (first_effect == _effects.end())
        {
            return;
        }
        first_effect->copy_target_to_texture();
    }

    void PostProcessor::draw(const engine::GameTime &draw_time)
    {
        for (auto effect = _effects.begin();; ++effect)
        {
            const auto next_effect = std::next(effect);
            if (next_effect != _effects.end())
            {
                effect->activate_render_target();
            }

            effect->shader().use();
            effect->use_input_textures();

            // render textured quad
            glBindVertexArray(_vao);
            glCheckError();
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glCheckError();
            glBindVertexArray(0);
            glCheckError();

            if (next_effect != _effects.end())
            {
                effect->copy_target_to_texture();
            }
            else
            {
                break;
            }
        }
    }

    void PostProcessor::activate_color_attachements(const PostProcessingEffect &effect) const
    {
    }
} // namespace engine::graphics
