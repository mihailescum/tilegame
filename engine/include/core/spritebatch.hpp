#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "graphicsdevice.hpp"
#include "texture2d.hpp"
#include "shader.hpp"

namespace engine
{
    class SpriteBatch
    {
    private:
        static const int NUM_SIMULT_TEXTURES = 1;
        static const int MAX_BATCH_SIZE = 4096;

        const int SPRITE_SIZE_VBO = 16;
        const int SPRITE_SIZE_EBO = 6;

        static const std::string VERTEX_SHADER_SOURCE;
        static const std::string FRAGMENT_SHADER_SOURCE;

        GraphicsDevice &_graphics_device;
        GLuint _vbo;
        GLuint _vao;
        GLuint _ebo;

        Shader _shader;

        glm::mat4 _wvp;
        glm::mat4 _projection;
        Texture2D _activeTextures[NUM_SIMULT_TEXTURES];
        std::vector<float> _sprite_data_vbo;
        std::vector<unsigned int> _sprite_data_ebo;

        int _num_active_textures;
        int _num_active_sprites;
        bool _has_begun;

        void flush();
        void add_sprite_data(const Texture2D &texture, const Rectangle &destination_rectangle, const Rectangle *const source_rectangle, const Color &color, float z);

    public:
        SpriteBatch(GraphicsDevice &graphics_device);
        SpriteBatch(const SpriteBatch &sprite_batch) = delete;
        virtual ~SpriteBatch();

        void create();
        void begin(const bool alpha_blending_enabled);
        void begin(const glm::mat4 &transform, const bool alpha_blending_enabled);
        void draw(const Texture2D &texture, const Rectangle &destination_rectangle, const Rectangle *const source_rectangle = nullptr, const Color &color = Color::WHITE, float z = 0.0);
        void end();
    };
} // namespace engine
