#include "graphics/postprocessingeffect.hpp"

#include "core/glerror.hpp"
#include "graphics/postprocessor.hpp"

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

    int PostProcessingEffect::add_color_attachments(std::size_t count)
    {
        if (!generate_buffers())
        {
            return 0;
        }

        return PostProcessor::add_color_attachments(count, _graphicsdevice.viewport().dimensions, _msfbo, _fbo, _color_attachments, _gl_color_attachments);
    }

    bool PostProcessingEffect::do_pass(bool bind_default_framebuffer)
    {
        // Finish up the previous pass
        if (_current_pass >= 0)
        {
            if (_msfbo != 0 && !bind_default_framebuffer)
            {
                const auto &dimensions = _graphicsdevice.viewport().dimensions;

                // now resolve multisampled color-buffer into intermediate FBO to store to texture
                PostProcessor::blit_framebuffer(_msfbo, _fbo, _gl_color_attachments, dimensions);
            }
        }

        // Quit if no passes left to do
        if (_current_pass == 1)
        {
            _current_pass = -1;
            return false;
        }

        // Start the new pass
        _current_pass++;

        // Bind the frame buffer
        if (bind_default_framebuffer)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, _msfbo);
        }
        glCheckError();

        // activate shader
        _shader.use();
        // activate input textures
        use_input_textures();

        return true;
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
