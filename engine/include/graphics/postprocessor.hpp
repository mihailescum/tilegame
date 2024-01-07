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
        GLuint _vao;
        std::vector<PostProcessingEffect> _effects;

        graphics::GraphicsDevice &_graphicsdevice;
        int initialize_vao();
        void activate_color_attachements(const PostProcessingEffect &effect) const;

    public:
        PostProcessor(graphics::GraphicsDevice &graphicsdevice) : _graphicsdevice(graphicsdevice) {}

        int initialize();
        void begin();
        void end();
        void draw(const engine::GameTime &draw_time);

        const std::vector<PostProcessingEffect> &effects() const { return _effects; }
        std::vector<PostProcessingEffect> &effects() { return _effects; }
    };
} // namespace engine::graphics
