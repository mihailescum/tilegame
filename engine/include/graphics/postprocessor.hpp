#pragma once

#include <vector>
#include <array>

#include "glad/glad.h"

#include "core/gametime.hpp"
#include "core/texture2d.hpp"
#include "core/shader.hpp"
#include "graphics/graphicsdevice.hpp"
#include "graphics/postprocessingeffect.hpp"

// See here for a reference tutorial: https://learnopengl.com/In-Practice/2D-Game/Postprocessing

namespace engine::graphics
{
    /**
     * @brief Owns and drives a chain of PostProcessingEffect passes (e.g. day/night
     * tint blending). The normal scene is rendered into this class's own multisampled
     * framebuffer between begin_scene()/end_scene(), resolved into a texture, and then
     * fed through the effect chain by apply_effects(), with the last effect writing to
     * the default framebuffer. Also provides the shared FBO setup/blit helpers used by
     * both itself and PostProcessingEffect.
     */
    class PostProcessor
    {
    private:
        GLuint _msfbo;
        GLuint _fbo;
        std::vector<std::unique_ptr<engine::Texture2D>> _color_attachments;
        std::vector<GLenum> _gl_color_attachments;

        GLuint _vao;
        std::vector<PostProcessingEffect> _effects;

        graphics::GraphicsDevice &_graphicsdevice;
        int initialize_vao();

        int generate_buffers();
        void activate_render_target(GLuint fbo) const;
        void copy_target_to_texture() const;

    public:
        PostProcessor(graphics::GraphicsDevice &graphicsdevice) : _graphicsdevice(graphicsdevice) {}

        /** @brief Sets up this PostProcessor's own framebuffers and the fullscreen quad VAO used to draw each effect's output. Must be called once before use. */
        int initialize();
        int add_color_attachments(std::size_t count);
        const engine::Texture2D &color_attachment_at(std::size_t index) const;

        /** @brief Shared FBO setup helper: creates `count` multisampled + resolve color attachments sized to `viewport_dimensions` on the given framebuffers. Used by both PostProcessor and PostProcessingEffect. */
        static int add_color_attachments(std::size_t count, const glm::ivec2 &viewport_dimensions, GLuint msfbo, GLuint fbo, std::vector<std::unique_ptr<engine::Texture2D>> &color_attachments, std::vector<GLenum> &gl_color_attachments);
        /** @brief Resolves each of `gl_color_attachments` from the multisampled `fbo1` into the sampleable `fbo2`. */
        static void blit_framebuffer(GLuint fbo1, GLuint fbo2, const std::vector<GLenum> &gl_color_attachments, const glm::ivec2 &viewport_dimensions);

        /** @brief Redirects subsequent scene rendering into this PostProcessor's multisampled framebuffer. No-op if there are no effects. */
        void begin_scene();
        /** @brief Resolves the scene framebuffer into a sampleable texture, ready to feed into apply_effects(). No-op if there are no effects. */
        void end_scene();
        /** @brief Runs the scene texture through the full effect chain in order, with the last effect rendering to the default framebuffer. */
        void apply_effects(const engine::GameTime &draw_time);

        const std::vector<PostProcessingEffect> &effects() const { return _effects; }
        std::vector<PostProcessingEffect> &effects() { return _effects; }
    };
} // namespace engine::graphics
