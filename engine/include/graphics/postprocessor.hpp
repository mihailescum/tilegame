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

        int initialize();
        int add_color_attachments(std::size_t count);
        const engine::Texture2D &color_attachment_at(std::size_t index) const;

        static int add_color_attachments(std::size_t count, const glm::ivec2 &viewport_dimensions, GLuint msfbo, GLuint fbo, std::vector<std::unique_ptr<engine::Texture2D>> &color_attachments, std::vector<GLenum> &gl_color_attachments);
        static void blit_framebuffer(GLuint fbo1, GLuint fbo2, const std::vector<GLenum> &gl_color_attachments, const glm::ivec2 &viewport_dimensions);

        void begin_scene();
        void end_scene();
        void apply_effects(const engine::GameTime &draw_time);

        const std::vector<PostProcessingEffect> &effects() const { return _effects; }
        std::vector<PostProcessingEffect> &effects() { return _effects; }
    };
} // namespace engine::graphics
