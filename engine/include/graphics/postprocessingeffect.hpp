#pragma once

#include <vector>
#include <memory>

#include "glad/glad.h"

#include "core/texture2d.hpp"
#include "core/shader.hpp"
#include "graphics/graphicsdevice.hpp"

namespace engine::graphics
{
    class PostProcessingEffect
    {
    private:
        GLuint _msfbo;
        GLuint _fbo;
        std::vector<std::unique_ptr<engine::Texture2D>> _color_attachments;
        std::vector<std::reference_wrapper<const engine::Texture2D>> _input_textures;
        std::vector<GLenum> _gl_color_attachments;

        const engine::Shader &_shader;
        const GraphicsDevice &_graphicsdevice;

        int generate_buffers();

    protected:
        int _current_pass;

    public:
        PostProcessingEffect(const graphics::GraphicsDevice &graphicsdevice, const engine::Shader &shader) : _graphicsdevice(graphicsdevice), _shader(shader), _msfbo(0), _fbo(0), _current_pass(-1) {}
        PostProcessingEffect(PostProcessingEffect &&) = default;
        virtual ~PostProcessingEffect() {}

        int add_color_attachments(std::size_t count);
        void use_input_textures() const;
        virtual bool do_pass(bool bind_default_framebuffer);

        const engine::Texture2D &color_attachment_at(std::size_t index) const;
        const engine::Shader &shader() const { return _shader; }
        const auto &input_textures() const { return _input_textures; }
        auto &input_textures() { return _input_textures; }
    };
} // namespace engine::graphics
