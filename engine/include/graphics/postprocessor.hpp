#pragma once

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
        GLuint _msfbo, _fbo; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
        GLuint _rbo;         // RBO is used for multisampled color buffer
        GLuint _vao;

        engine::Texture2D _texture;
        engine::Shader _shader;

        const std::string VERTEX_SHADER_SOURCE{
#include "graphics/shader/postprocessor.vert"
        };

        const std::string FRAGMENT_SHADER_SOURCE{
#include "graphics/shader/postprocessor.frag"
        };

        graphics::GraphicsDevice &_graphicsdevice;

        int generate_buffers();
        int initialize_rbo();
        int initialize_fbo();
        int initialize_vao();

    public:
        PostProcessor(graphics::GraphicsDevice &graphicsdevice) : _graphicsdevice(graphicsdevice) {}

        int initialize();
        void begin();
        void end();
        void draw(const engine::GameTime &draw_time);
    };
} // namespace engine::graphics
