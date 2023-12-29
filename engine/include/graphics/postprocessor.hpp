#pragma once

#include <vector>
#include <array>

#include "glad/glad.h"

#include "core/gametime.hpp"
#include "core/texture2d.hpp"
#include "core/shader.hpp"
#include "graphics/graphicsdevice.hpp"

// See here for a reference tutorial: https://learnopengl.com/In-Practice/2D-Game/Postprocessing

namespace engine::graphics
{
    class PostProcessor
    {
    private:
        // Since we allow multiple effects, we need to use multibuffering
        static const short NUM_BUFFERS = 2;
        std::array<GLuint, NUM_BUFFERS> _msfbo, _fbo; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
        std::array<GLuint, NUM_BUFFERS> _rbo;         // RBO is used for multisampled color buffer
        GLuint _vao;

        short _current_pass;

        std::array<engine::Texture2D, NUM_BUFFERS> _texture;
        std::vector<engine::Shader *> _shaders;
        graphics::GraphicsDevice &_graphicsdevice;

        int generate_buffers();
        int initialize_rbo();
        int initialize_fbo();
        int initialize_vao();

    public:
        PostProcessor(graphics::GraphicsDevice &graphicsdevice) : _graphicsdevice(graphicsdevice), _current_pass(0) {}

        int initialize();
        void begin();
        void end();
        void draw(const engine::GameTime &draw_time);

        const std::vector<engine::Shader *> &shaders() const { return _shaders; }
        std::vector<engine::Shader *> &shaders() { return _shaders; }
    };
} // namespace engine::graphics
