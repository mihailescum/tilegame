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
        struct SpriteData
        {
            engine::Rectangle destination_rectangle;
            engine::Rectangle source_rectangle;
            engine::Color color;
            GLuint gl_texture;
            float z;

            SpriteData() {}
        };

        static const int MAX_BATCH_SIZE = 16384;

        static const int SPRITE_SIZE_VBO = 32;
        static const int SPRITE_SIZE_EBO = 6;

        static const std::string VERTEX_SHADER_SOURCE;
        static const std::string FRAGMENT_SHADER_SOURCE;

        GraphicsDevice &_graphics_device;
        GLuint _vbo;
        GLuint _vao;
        GLuint _ebo;

        Shader _shader;

        glm::mat4 _wvp;
        glm::mat4 _projection;
        std::vector<SpriteData> _sprite_data;
        std::array<GLfloat, MAX_BATCH_SIZE * SPRITE_SIZE_VBO> _sprite_data_vbo;
        std::array<GLuint, MAX_BATCH_SIZE * SPRITE_SIZE_EBO> _sprite_data_ebo;

        std::size_t _num_active_sprites;
        std::size_t _current_batch_start;
        bool _has_begun;

        void flush();
        void add_sprite_data(const Texture2D &texture, const Rectangle &destination_rectangle, const Rectangle *const source_rectangle, const Color &color, float z);
        GLuint set_buffer_data(std::size_t &batch_size);

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
