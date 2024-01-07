#include "graphics/postprocessingeffect.hpp"

#include "core/glerror.hpp"

namespace engine::graphics
{
    int PostProcessingEffect::generate_buffers()
    {
        glGenFramebuffers(1, &_msfbo);
        glCheckError();
        glGenFramebuffers(1, &_fbo);
        glCheckError();

        return 1;
    }

    int PostProcessingEffect::add_color_attachments(int count)
    {
        if (!generate_buffers())
        {
            return 0;
        }

        const auto &viewport_size = _graphicsdevice.viewport().size;
        for (int i = 0; i < count; i++)
        {
            auto texture = std::make_unique<engine::Texture2D>();
            texture->create_texture_from_raw_data(viewport_size.x, viewport_size.y, NULL);

            // Set attachment on FBO
            glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
            glCheckError();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->gl_texture(), 0); // attach texture to framebuffer as its color attachment
            glCheckError();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glCheckError();

            // Generate RBO
            GLuint rbo;
            glGenRenderbuffers(1, &rbo);

            // Set attachment on MSFBOGLint max_samples;
            GLint max_samples;
            glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
            glCheckError();

            glBindFramebuffer(GL_FRAMEBUFFER, _msfbo);
            glCheckError();
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glCheckError();
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, max_samples, GL_RGB, viewport_size.x, viewport_size.y); // allocate storage for render buffer object
            glCheckError();
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo); // attach MS render buffer object to framebuffer
            glCheckError();

            _color_attachments.push_back(std::move(texture));
        }

        // Check if the FBO is complete
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glCheckError();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Log::e("ERROR::POSTPROCESSOR: Failed to initialize MSFBO");
            return 0;
        }

        // Check if the MSFBO is complete
        glBindFramebuffer(GL_FRAMEBUFFER, _msfbo);
        glCheckError();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Log::e("ERROR::POSTPROCESSOR: Failed to initialize MSFBO");
            return 0;
        }
        return 1;
    }

    void PostProcessingEffect::activate_render_target() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _msfbo);
        glCheckError();

        // activate shader
        _shader.use();
    }

    void PostProcessingEffect::copy_target_to_texture() const
    {
        if (_msfbo == 0)
        {
            return;
        }

        const auto &size = _graphicsdevice.viewport().size;

        // now resolve multisampled color-buffer into intermediate FBO to store to texture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _msfbo);
        glCheckError();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
        glCheckError();
        glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glCheckError();
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // binds both READ and WRITE framebuffer to default framebuffer
        glCheckError();
    }

    const engine::Texture2D &PostProcessingEffect::color_attachment_at(std::size_t index) const
    {
        if (index >= _color_attachments.size())
        {
            throw "Index out of range";
        }
        return *_color_attachments.at(index);
    }

    void PostProcessingEffect::use_input_textures() const
    {
        for (unsigned int i = 0; i < _input_textures.size(); ++i)
        {
            _input_textures[i].get().use(i);
        }
    }
} // namespace engine::graphics
