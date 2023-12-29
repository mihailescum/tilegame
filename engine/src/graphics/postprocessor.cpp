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
        if (!initialize_rbo())
        {
            return 0;
        }
        if (!initialize_fbo())
        {
            return 0;
        }
        if (!initialize_vao())
        {
            return 0;
        }

        Shader *shader1 = new Shader();
        shader1->compile(PostProcessor::VERTEX_SHADER_SOURCE, "", PostProcessor::FRAGMENT_SHADER_SOURCE);
        _shader.push_back(shader1);
        _shader.push_back(shader1);
        _shader.push_back(shader1);

        return 1;
    }

    int PostProcessor::generate_buffers()
    {
        // initialize renderbuffer/framebuffer objects
        for (std::size_t i = 0; i < NUM_BUFFERS; ++i)
        {
            glGenFramebuffers(1, &_msfbo[i]);
            glCheckError();
            glGenFramebuffers(1, &_fbo[i]);
            glCheckError();
            glGenRenderbuffers(1, &_rbo[i]);
            glCheckError();
        }

        return 1;
    }

    int PostProcessor::initialize_rbo()
    {
        int width = _graphicsdevice.viewport().width;
        int height = _graphicsdevice.viewport().height;

        GLint max_samples;
        glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
        glCheckError();

        // initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
        for (std::size_t i = 0; i < NUM_BUFFERS; ++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, _msfbo[i]);
            glCheckError();
            glBindRenderbuffer(GL_RENDERBUFFER, _rbo[i]);
            glCheckError();
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, max_samples, GL_RGB, width, height); // allocate storage for render buffer object
            glCheckError();

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _rbo[i]); // attach MS render buffer object to framebuffer
            glCheckError();
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                Log::e("ERROR::POSTPROCESSOR: Failed to initialize MSFBO");
                return 0;
            }
        }
        return 1;
    }

    int PostProcessor::initialize_fbo()
    {
        int width = _graphicsdevice.viewport().width;
        int height = _graphicsdevice.viewport().height;

        // also initialize the FBOs/textures to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
        for (std::size_t i = 0; i < NUM_BUFFERS; ++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, _fbo[i]);
            glCheckError();
            _texture[i].create_texture_from_raw_data(width, height, NULL);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture[i].gl_texture(), 0); // attach texture to framebuffer as its color attachment
            glCheckError();
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                Log::e("ERROR::POSTPROCESSOR: Failed to initialize FBO");
                return 0;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glCheckError();
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
        if (_current_pass == _shader.size())
        {
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, _msfbo[_current_pass & 1]); // fast mod 2
        glCheckError();
    }

    void PostProcessor::end()
    {
        if (_current_pass == _shader.size())
        {
            return;
        }

        int width = _graphicsdevice.viewport().width;
        int height = _graphicsdevice.viewport().height;

        // now resolve multisampled color-buffer into intermediate FBO to store to texture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _msfbo[_current_pass & 1]);
        glCheckError();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo[_current_pass & 1]);
        glCheckError();
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glCheckError();
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // binds both READ and WRITE framebuffer to default framebuffer
        glCheckError();
    }

    void PostProcessor::draw(const engine::GameTime &draw_time)
    {
        for (auto s : _shader)
        {
            _current_pass++;

            begin();

            // activate shader
            s->use();

            // render textured quad
            _texture[(_current_pass - 1) & 1].use(0);

            glBindVertexArray(_vao);
            glCheckError();
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glCheckError();
            glBindVertexArray(0);
            glCheckError();

            end();
        }

        _current_pass = 0;
    }
} // namespace engine::graphics
