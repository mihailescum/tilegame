#pragma once

#include <vector>
#include <memory>

#include "graphicsdevice.hpp"
#include "texture2D.hpp"
#include "shader.hpp"

namespace engine
{
    class SpriteBatch
    {
    private:
        static const int NUM_SIMULT_TEXTURES = 1;
        static const int MAX_BATCH_SIZE = 4096;

        static const std::string VERTEX_SHADER_SOURCE;
        static const std::string FRAGMENT_SHADER_SOURCE;

        GraphicsDevice &_graphics_device;
        GLuint _vbo;
        GLuint _vao;

        Shader _shader;

        glm::mat4 _wvp;
        Texture2D _activeTextures[NUM_SIMULT_TEXTURES];
        std::vector<float> _sprite_data;
        int _num_active_textures;
        int _num_active_sprites;
        bool _has_begun;

        void flush();
        void add_sprite_data(const Texture2D &texture, const Rectangle &destination_rectangle, const Rectangle *source_rectangle);

    public:
        SpriteBatch(GraphicsDevice &graphics_device);
        SpriteBatch(const SpriteBatch &sprite_batch) = delete;
        virtual ~SpriteBatch();

        void create();
        void begin(const bool alpha_blending_enabled);
        void begin(const glm::mat4 &transform, const bool alpha_blending_enabled);
        void draw(const Texture2D &texture, const Rectangle &destination_rectangle, const Color &color);
        void draw(const Texture2D &texture, const Rectangle &destination_rectangle, const Rectangle *source_rectangle, const Color &color);
        void end();
    };
} // namespace engine
