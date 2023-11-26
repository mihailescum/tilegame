#include "glad/glad.h"

#include <glm/gtc/matrix_transform.hpp>

#include "core/spritebatch.hpp"

namespace engine
{
    SpriteBatch::SpriteBatch(GraphicsDevice &graphicsDevice)
        : _graphics_device(graphicsDevice),
          _num_active_sprites(0),
          _has_begun(false),
          _sprite_data(MAX_BATCH_SIZE)
    {
    }

    SpriteBatch::~SpriteBatch()
    {
        if (this->_vbo != 0)
            glDeleteBuffers(1, &_vbo);
        if (this->_ebo != 0)
            glDeleteBuffers(1, &_ebo);
        if (this->_vao != 0)
            glDeleteVertexArrays(1, &_vao);
    }

    void SpriteBatch::create()
    {
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ebo);
        glGenVertexArrays(1, &_vao);

        _shader.compile(SpriteBatch::VERTEX_SHADER_SOURCE, "", SpriteBatch::FRAGMENT_SHADER_SOURCE);

        const Viewport &viewport = this->_graphics_device.viewport();
        _projection = glm::ortho(
            static_cast<float>(viewport.x),
            static_cast<float>(viewport.width),
            static_cast<float>(viewport.height),
            static_cast<float>(viewport.y),
            -1.0f,
            1.0f);
    }

    void SpriteBatch::begin(const bool alpha_blending_enabled)
    {
        glm::mat4 transform(1.0);
        this->begin(transform, alpha_blending_enabled);
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
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        _num_active_sprites = 0;
    }

    void SpriteBatch::end()
    {
        if (!this->_has_begun)
        {
            throw "You have to call 'begin()' on a SpriteBatch first.";
        }
        _current_batch_start = 0;
        while (_current_batch_start < _num_active_sprites)
        {
            flush();
        }

        glDisable(GL_BLEND);

        this->_has_begun = false;
    }

    void SpriteBatch::draw(
        const Texture2D &texture,
        const Rectangle &destination_rectangle,
        const Rectangle *const source_rectangle,
        const Color &color,
        float z)
    {
        if (!this->_has_begun)
        {
            throw "You have to call 'begin()' on a SpriteBatch first.";
        }

        this->add_sprite_data(texture, destination_rectangle, source_rectangle, color, z);
        this->_num_active_sprites++;
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
        data.color = color;
        data.gl_texture = texture.gl_texture();
        data.destination_rectangle = destination_rectangle;
        if (source_rectangle)
        {
            data.source_rectangle = *source_rectangle;
            data.source_rectangle.x /= texture.width();
            data.source_rectangle.y /= texture.height();
            data.source_rectangle.width /= texture.width();
            data.source_rectangle.height /= texture.height();
        }
        else
        {
            data.source_rectangle = engine::Rectangle(0.0, 0.0, 1.0, 1.0);
        }
        data.z = z;
    }

    void SpriteBatch::flush()
    {
        glBindVertexArray(_vao);

        std::size_t batch_size;
        auto active_texture = set_buffer_data(batch_size);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(4 * sizeof(float)));
        glEnableVertexAttribArray(1);

        _shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, active_texture);

        _shader.set_int("Texture", 0);
        _shader.set_mat4("WVP", _wvp);

        // glDrawArrays(GL_TRIANGLES, 0, batch_size * 6);
        glDrawElements(GL_TRIANGLES, batch_size * 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        _current_batch_start += batch_size;
    }

    GLuint SpriteBatch::set_buffer_data(std::size_t &batch_size)
    {
        GLuint active_texture = 0;
        batch_size = _num_active_sprites - _current_batch_start;
        if (batch_size > MAX_BATCH_SIZE)
        {
            batch_size = MAX_BATCH_SIZE;
        }

        auto _sprite_data_ptr = &(_sprite_data[_current_batch_start]);
        auto _sprite_data_vbo_ptr = &(_sprite_data_vbo[0]);
        auto _sprite_data_ebo_ptr = &(_sprite_data_ebo[0]);
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

            /*if (offset_vbo >= _sprite_data_vbo.size()) // Grow the VBO if necessary
            {
                for (int j = 0; j < SPRITE_SIZE_VBO; ++j)
                {
                    _sprite_data_vbo.push_back(0.0);
                }
            }*/

            auto &dest_rect = sprite_data.destination_rectangle;
            auto &source_rect = sprite_data.source_rectangle;
            auto &color = sprite_data.color;

            /*  0 -- 1
                |   /|
                |  / |
                | /  |
                2 -- 3
            */

            // Top Left
            *(_sprite_data_vbo_ptr++) = dest_rect.x;
            *(_sprite_data_vbo_ptr++) = dest_rect.y;
            *(_sprite_data_vbo_ptr++) = source_rect.x;
            *(_sprite_data_vbo_ptr++) = source_rect.y;
            *(_sprite_data_vbo_ptr++) = color.r;
            *(_sprite_data_vbo_ptr++) = color.g;
            *(_sprite_data_vbo_ptr++) = color.b;
            *(_sprite_data_vbo_ptr++) = color.a;

            // Top Right
            *(_sprite_data_vbo_ptr++) = dest_rect.x + dest_rect.width;
            *(_sprite_data_vbo_ptr++) = dest_rect.y;
            *(_sprite_data_vbo_ptr++) = source_rect.x + source_rect.width;
            *(_sprite_data_vbo_ptr++) = source_rect.y;
            *(_sprite_data_vbo_ptr++) = color.r;
            *(_sprite_data_vbo_ptr++) = color.g;
            *(_sprite_data_vbo_ptr++) = color.b;
            *(_sprite_data_vbo_ptr++) = color.a;

            // Bottom Left
            *(_sprite_data_vbo_ptr++) = dest_rect.x;
            *(_sprite_data_vbo_ptr++) = dest_rect.y + dest_rect.height;
            *(_sprite_data_vbo_ptr++) = source_rect.x;
            *(_sprite_data_vbo_ptr++) = source_rect.y + source_rect.height;
            *(_sprite_data_vbo_ptr++) = color.r;
            *(_sprite_data_vbo_ptr++) = color.g;
            *(_sprite_data_vbo_ptr++) = color.b;
            *(_sprite_data_vbo_ptr++) = color.a;

            // Bottom Right
            *(_sprite_data_vbo_ptr++) = dest_rect.x + dest_rect.width;
            *(_sprite_data_vbo_ptr++) = dest_rect.y + dest_rect.height;
            *(_sprite_data_vbo_ptr++) = source_rect.x + source_rect.width;
            *(_sprite_data_vbo_ptr++) = source_rect.y + source_rect.height;
            *(_sprite_data_vbo_ptr++) = color.r;
            *(_sprite_data_vbo_ptr++) = color.g;
            *(_sprite_data_vbo_ptr++) = color.b;
            *(_sprite_data_vbo_ptr++) = color.a;

            *(_sprite_data_ebo_ptr++) = i * 8;
            *(_sprite_data_ebo_ptr++) = i * 8 + 2;
            *(_sprite_data_ebo_ptr++) = i * 8 + 4;
            *(_sprite_data_ebo_ptr++) = i * 8 + 4;
            *(_sprite_data_ebo_ptr++) = i * 8 + 2;
            *(_sprite_data_ebo_ptr++) = i * 8 + 6;
        }

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, batch_size * SPRITE_SIZE_VBO * sizeof(GLfloat), &_sprite_data_vbo[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch_size * SPRITE_SIZE_EBO * sizeof(GLuint), &_sprite_data_ebo[0], GL_STATIC_DRAW);

        return active_texture;
    }

    const std::string SpriteBatch::VERTEX_SHADER_SOURCE = R"(
    #version 330 core
    layout (location = 0) in vec4 vertex;
    layout (location = 1) in vec4 color;
    
    out vec2 TexCoord;
    out vec4 VertexColor;
    
    uniform mat4 WVP;
    
    void main()
    {
        gl_Position = WVP * vec4(vertex.xy, 0.0, 1.0);
        TexCoord = vertex.zw;
        VertexColor = color;
    })";

    const std::string SpriteBatch::FRAGMENT_SHADER_SOURCE = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec2 TexCoord;
    in vec4 VertexColor;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec4 color = texture(Texture, TexCoord) * VertexColor;
        FragColor = color;
    })";
} // namespace engine