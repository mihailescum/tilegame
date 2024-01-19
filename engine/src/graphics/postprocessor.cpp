#include "graphics/postprocessor.hpp"

#include "core/log.hpp"
#include "core/glerror.hpp"

namespace engine::graphics
{
    int PostProcessor::initialize()
    {
        if (!generate_buffers())
        {
            return 0;
        }
        if (!initialize_vao())
        {
            return 0;
        }

        return 1;
    }

    int PostProcessor::generate_buffers()
    {
        glGenFramebuffers(1, &_msfbo);
        glCheckError();
        glGenFramebuffers(1, &_fbo);
        glCheckError();

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

    void PostProcessor::begin_scene()
    {
        if (_effects.size() == 0)
        {
            return;
        }

        activate_render_target(_msfbo);
        _graphicsdevice.clear(engine::Color::BLACK);
    }

    void PostProcessor::end_scene()
    {
        if (_effects.size() == 0)
        {
            return;
        }
        copy_target_to_texture();
    }

    void PostProcessor::apply_effects(const engine::GameTime &draw_time)
    {
        if (_effects.size() == 0)
        {
            return;
        }

        for (auto &effect : _effects)
        {
            bool bind_default_framebuffer = &effect == &_effects.back();

            while (effect.do_pass(bind_default_framebuffer))
            {
                if (!bind_default_framebuffer)
                {
                    _graphicsdevice.clear(engine::Color::BLACK);
                }

                // render textured quad
                glBindVertexArray(_vao);
                glCheckError();
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glCheckError();
                glBindVertexArray(0);
                glCheckError();
            }
        }
    }

    int PostProcessor::add_color_attachments(std::size_t count)
    {
        return add_color_attachments(count, _graphicsdevice.viewport().dimensions, _msfbo, _fbo, _color_attachments, _gl_color_attachments);
    }

    int PostProcessor::add_color_attachments(std::size_t count, const glm::ivec2 &framebuffer_dimensions, GLuint msfbo, GLuint fbo, std::vector<std::unique_ptr<engine::Texture2D>> &color_attachments, std::vector<GLenum> &gl_color_attachments)
    {
        // Setup FBO
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glCheckError();

        gl_color_attachments.resize(count);
        for (int i = 0; i < count; i++)
        {
            gl_color_attachments[i] = GL_COLOR_ATTACHMENT0 + i;

            auto texture = std::make_unique<engine::Texture2D>();
            texture->create_texture_from_raw_data(framebuffer_dimensions.x, framebuffer_dimensions.y, NULL);

            glFramebufferTexture2D(GL_FRAMEBUFFER, gl_color_attachments[i], GL_TEXTURE_2D, *texture, 0); // attach texture to framebuffer as its color attachment
            glCheckError();

            color_attachments.push_back(std::move(texture));
        }

        // Check if the FBO is complete
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glCheckError();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Log::e("ERROR::POSTPROCESSOR: Failed to initialize MSFBO");
            return 0;
        }

        // Setup MSFBO

        // Set attachment on MSFBO
        GLint max_samples;
        glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
        glCheckError();

        glBindFramebuffer(GL_FRAMEBUFFER, msfbo);
        glCheckError();

        for (int i = 0; i < count; i++)
        {
            // Generate RBO
            GLuint rbo;
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glCheckError();
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, max_samples, GL_RGB, framebuffer_dimensions.x, framebuffer_dimensions.y); // allocate storage for render buffer object
            glCheckError();
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, gl_color_attachments[i], GL_RENDERBUFFER, rbo); // attach MS render buffer object to framebuffer
            glCheckError();
        }

        glDrawBuffers(gl_color_attachments.size(), &gl_color_attachments[0]);
        glCheckError();

        // Check if the MSFBO is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Log::e("ERROR::POSTPROCESSOR: Failed to initialize MSFBO");
            return 0;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glCheckError();
        return 1;
    }

    void PostProcessor::activate_render_target(GLuint fbo) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glCheckError();
    }

    void PostProcessor::copy_target_to_texture() const
    {
        if (_msfbo == 0)
        {
            return;
        }

        const auto &dimensions = _graphicsdevice.viewport().dimensions;

        // now resolve multisampled color-buffer into intermediate FBO to store to texture
        blit_framebuffer(_msfbo, _fbo, _gl_color_attachments, dimensions);
    }

    void PostProcessor::blit_framebuffer(GLuint read_buffer, GLuint draw_buffer, const std::vector<GLenum> &gl_color_attachments, const glm::ivec2 &framebuffer_dimensions)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, read_buffer);
        glCheckError();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_buffer);
        glCheckError();

        for (const auto gl_color_attachment : gl_color_attachments)
        {
            glReadBuffer(gl_color_attachment);
            glCheckError();
            glDrawBuffer(gl_color_attachment);
            glCheckError();
            glBlitFramebuffer(0, 0, framebuffer_dimensions.x, framebuffer_dimensions.y, 0, 0, framebuffer_dimensions.x, framebuffer_dimensions.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glCheckError();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // binds both READ and WRITE framebuffer to default framebuffer
        glCheckError();
    }

    const engine::Texture2D &PostProcessor::color_attachment_at(std::size_t index) const
    {
        if (index >= _color_attachments.size())
        {
            throw "Index out of range";
        }
        return *_color_attachments.at(index);
    }
} // namespace engine::graphics
