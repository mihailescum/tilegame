#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "core/texture2d.hpp"
#include "core/shader.hpp"
#include "graphics/graphicsdevice.hpp"
#include "graphics/spritedata.hpp"
#include "graphics/vertexpositiontexturecolor.hpp"

namespace engine::graphics
{
    class SpriteBatch
    {
    private:
        using ebo_type = GLushort;
        static const ebo_type MAX_BATCH_SIZE = 65535U / 6;
        static const int ELEMENT_SIZE = sizeof(ebo_type);

        const std::string VERTEX_SHADER_SOURCE{
#include "graphics/shader/spritebatch.vert"
        };

        const std::string FRAGMENT_SHADER_SOURCE{
#include "graphics/shader/spritebatch.frag"
        };

        const GraphicsDevice &_graphicsdevice;
        GLuint _vbo;
        GLuint _vao;
        GLuint _ebo;

        Shader _shader;

        glm::mat4 _wvp;
        glm::mat4 _projection;
        std::vector<SpriteData> _sprite_data;
        std::vector<VertexPositionTextureColor> _sprite_data_vbo;
        std::vector<ebo_type> _sprite_data_ebo;

        std::size_t _num_active_sprites;
        std::size_t _current_batch_start;
        bool _has_begun;

        void flush();
        void add_sprite_data(const Texture2D &texture, const Rectangle &destination_rectangle, const Rectangle *const source_rectangle, const Color &color, float z);
        std::size_t update_vbo(GLuint &active_texture);
        void create_ebo();
        void create_vbo();
        void create_vao();

    public:
        SpriteBatch(const GraphicsDevice &graphics_device);
        SpriteBatch(const SpriteBatch &sprite_batch) = delete;
        virtual ~SpriteBatch();

        void create();
        void begin(const bool alpha_blending_enabled);
        void begin(const glm::mat4 &transform, const bool alpha_blending_enabled);
        void draw(const Texture2D &texture, const Rectangle &destination_rectangle, const Rectangle *const source_rectangle = nullptr, const Color &color = Color::WHITE, float z = 0.0);
        void end();
    };
} // namespace engine
