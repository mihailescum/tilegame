#include "graphics/spritebatch.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "glad/glad.h"

#include "core/glerror.hpp"

namespace engine::graphics
{
    SpriteBatch::SpriteBatch(GraphicsDevice &graphicsDevice)
        : _graphicsdevice(graphicsDevice),
          _num_active_sprites(0),
          _has_begun(false),
          _sprite_data(MAX_BATCH_SIZE)
    {
    }

    SpriteBatch::~SpriteBatch()
    {
        if (_vbo != 0)
            glDeleteBuffers(1, &_vbo);
        if (_ebo != 0)
            glDeleteBuffers(1, &_ebo);
        if (_vao != 0)
            glDeleteVertexArrays(1, &_vao);
    }

    void SpriteBatch::create()
    {
        create_vbo();
        create_ebo();
        create_vao();

        _shader.compile(SpriteBatch::VERTEX_SHADER_SOURCE, "", SpriteBatch::FRAGMENT_SHADER_SOURCE);

        _shader.use();
        _shader.set("Texture", 0);

        const Viewport &viewport = _graphicsdevice.viewport();
        _projection = glm::ortho(
            static_cast<float>(viewport.position.x),
            static_cast<float>(viewport.size.x),
            static_cast<float>(viewport.size.y),
            static_cast<float>(viewport.position.y),
            -1.0f,
            1.0f);
    }

    void SpriteBatch::create_vbo()
    {
        _sprite_data_vbo.resize(MAX_BATCH_SIZE * 4);

        glGenBuffers(1, &_vbo);
        glCheckError();

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glCheckError();
        // Allocate an empty buffer of the max size
        glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_SIZE * sizeof(VertexPositionTextureColor) * 4, NULL, GL_DYNAMIC_DRAW);
        glCheckError();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glCheckError();
    }

    void SpriteBatch::create_vao()
    {
        glGenVertexArrays(1, &_vao);
        glCheckError();

        glBindVertexArray(_vao);
        glCheckError();

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glCheckError();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glCheckError();

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionTextureColor), (void *)offsetof(VertexPositionTextureColor, position));
        glEnableVertexAttribArray(0);
        glCheckError();
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPositionTextureColor), (void *)offsetof(VertexPositionTextureColor, uv));
        glEnableVertexAttribArray(1);
        glCheckError();
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPositionTextureColor), (void *)offsetof(VertexPositionTextureColor, color));
        glEnableVertexAttribArray(2);
        glCheckError();

        glBindVertexArray(0);
        glCheckError();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glCheckError();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glCheckError();
    }

    void SpriteBatch::create_ebo()
    {
        /*  0 -- 1
            |\   |
            | \  |
            |  \ |
            2 -- 3
        */
        _sprite_data_ebo.resize(MAX_BATCH_SIZE * 6);
        for (std::size_t i = 0; i < MAX_BATCH_SIZE; ++i)
        {
            _sprite_data_ebo[i * 6] = i * 4;
            _sprite_data_ebo[i * 6 + 1] = i * 4 + 1;
            _sprite_data_ebo[i * 6 + 2] = i * 4 + 3;
            _sprite_data_ebo[i * 6 + 3] = i * 4;
            _sprite_data_ebo[i * 6 + 4] = i * 4 + 3;
            _sprite_data_ebo[i * 6 + 5] = i * 4 + 2;
        }

        glGenBuffers(1, &_ebo);
        glCheckError();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glCheckError();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_BATCH_SIZE * ELEMENT_SIZE * 6, &_sprite_data_ebo[0], GL_STATIC_DRAW);
        glCheckError();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glCheckError();
    }

    void SpriteBatch::begin(const bool alpha_blending_enabled)
    {
        glm::mat4 transform(1.0);
        begin(transform, alpha_blending_enabled);
    }

    void SpriteBatch::begin(const glm::mat4 &transform, const bool alpha_blending_enabled)
    {
        if (_has_begun)
        {
            throw "'begin()' was already called. Call 'end()' first.";
        }
        _has_begun = true;

        _wvp = _projection * transform;

        if (alpha_blending_enabled)
        {
            glEnable(GL_BLEND);
            glCheckError();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glCheckError();
        }
        _num_active_sprites = 0;
    }

    void SpriteBatch::end()
    {
        if (!_has_begun)
        {
            throw "You have to call 'begin()' on a SpriteBatch first.";
        }
        _current_batch_start = 0;
        while (_current_batch_start < _num_active_sprites)
        {
            flush();
        }

        glDisable(GL_BLEND);

        _has_begun = false;
    }

    void SpriteBatch::draw(
        const Texture2D &texture,
        const Rectangle &destination_rectangle,
        const Rectangle *const source_rectangle,
        const Color &color,
        float z)
    {
        if (!_has_begun)
        {
            throw "You have to call 'begin()' on a SpriteBatch first.";
        }

        add_sprite_data(texture, destination_rectangle, source_rectangle, color, z);
        _num_active_sprites++;
    }

    void SpriteBatch::add_sprite_data(
        const Texture2D &texture,
        const Rectangle &destination_rectangle,
        const Rectangle *const source_rectangle,
        const Color &color,
        float z)
    {
        if (_num_active_sprites >= _sprite_data.size())
        {
            _sprite_data.push_back(SpriteData());
        }
        auto &data = _sprite_data[_num_active_sprites];
        data.set(texture, destination_rectangle, source_rectangle, color, z);
    }

    void SpriteBatch::flush()
    {
        GLuint active_texture;
        std::size_t batch_size = update_vbo(active_texture);

        _shader.use();

        glActiveTexture(GL_TEXTURE0);
        glCheckError();
        glBindTexture(GL_TEXTURE_2D, active_texture);
        glCheckError();

        _shader.set("WVP", _wvp);

        glBindVertexArray(_vao);
        glCheckError();

        glDrawElements(GL_TRIANGLES, batch_size * 6, GL_UNSIGNED_SHORT, 0);
        glCheckError();

        glBindVertexArray(0);
        glCheckError();

        _current_batch_start += batch_size;
    }

    std::size_t SpriteBatch::update_vbo(GLuint &active_texture)
    {
        active_texture = 0;
        std::size_t batch_size = _num_active_sprites - _current_batch_start;
        if (batch_size > MAX_BATCH_SIZE)
        {
            batch_size = MAX_BATCH_SIZE;
        }

        auto _sprite_data_ptr = &(_sprite_data[_current_batch_start]);
        auto _sprite_data_vbo_ptr = &(_sprite_data_vbo[0]);
        for (std::size_t i = 0; i < batch_size; ++i)
        {
            auto &sprite_data = *(_sprite_data_ptr++);
            if (active_texture == 0)
            {
                active_texture = sprite_data.gl_texture;
            }
            else if (active_texture != sprite_data.gl_texture) // Need to switch texture!
            {
                batch_size = i; // Batch size is number of elements until this one
                break;
            }

            *(_sprite_data_vbo_ptr++) = sprite_data.top_left;
            *(_sprite_data_vbo_ptr++) = sprite_data.top_right;
            *(_sprite_data_vbo_ptr++) = sprite_data.bottom_left;
            *(_sprite_data_vbo_ptr++) = sprite_data.bottom_right;
        }

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glCheckError();
        // Orphan the vertex buffer
        // Refer to https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
        glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_SIZE * sizeof(VertexPositionTextureColor) * 4, NULL, GL_DYNAMIC_DRAW);
        glCheckError();
        // Update the needed subregion of the buffer
        glBufferSubData(GL_ARRAY_BUFFER, 0, batch_size * sizeof(VertexPositionTextureColor) * 4, &_sprite_data_vbo[0]);
        glCheckError();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glCheckError();

        return batch_size;
    }
} // namespace engine::graphics