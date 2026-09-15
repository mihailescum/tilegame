#pragma once

#include <vector>
#include <memory>

#include "glad/glad.h"

#include "core/texture2d.hpp"
#include "core/shader.hpp"
#include "graphics/graphicsdevice.hpp"

namespace engine::graphics
{
    /**
     * @brief A single shader-driven post-processing pass, owned and chained by a
     * PostProcessor. Renders its input textures (typically the previous effect's
     * color attachment) through a shader into its own multisampled framebuffer,
     * which is resolved into sampleable color attachment textures for the next
     * effect in the chain (or the default framebuffer for the last effect).
     */
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

        /** @brief Allocates `count` multisampled + resolve color attachment textures sized to the current viewport. */
        int add_color_attachments(std::size_t count);
        void use_input_textures() const;
        /**
         * @brief Advances the effect by one render pass. Intended to be called in a loop
         * by PostProcessor::apply_effects until it returns false; resolves the previous
         * pass's multisampled buffer into the effect's color attachments, then binds the
         * framebuffer for the next pass (the default framebuffer if `bind_default_framebuffer`
         * is set) and activates the shader and input textures. Currently supports at most
         * two passes per effect.
         */
        virtual bool do_pass(bool bind_default_framebuffer);

        const engine::Texture2D &color_attachment_at(std::size_t index) const;
        const engine::Shader &shader() const { return _shader; }
        /** @brief Textures sampled by this effect's shader, bound to texture units in order by use_input_textures(). Populated by the caller before do_pass(), typically with a preceding effect's color attachment. */
        const auto &input_textures() const { return _input_textures; }
        auto &input_textures() { return _input_textures; }
    };
} // namespace engine::graphics
