#include "glad/glad.h"

#include <glm/gtc/matrix_transform.hpp>

#include "core/spritebatch.hpp"

namespace engine
{
    SpriteBatch::SpriteBatch(GraphicsDevice &graphicsDevice)
        : _graphics_device(graphicsDevice),
          _num_active_textures(0),
          _num_active_sprites(0),
          _has_begun(false)
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

        const Viewport &viewport = this->_graphics_device.get_viewport();
        _projection = glm::ortho(
            static_cast<float>(viewport.x),
            static_cast<float>(viewport.width),
            static_cast<float>(viewport.height),
            static_cast<float>(viewport.y),
            -1.0f,
            1.0f);
    }

    void SpriteBatch::begin(const bool alphaBlendingEnabled)
    {
        glm::mat4 transform(1.0);
        this->begin(transform, alphaBlendingEnabled);
    }

    void SpriteBatch::begin(const glm::mat4 &transform, const bool alphaBlendingEnabled)
    {
        if (this->_has_begun)
        {
            throw "'begin()' was already called. Call 'end()' first.";
        }
        this->_has_begun = true;

        this->_wvp = _projection * transform;

        if (alphaBlendingEnabled)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }

    void SpriteBatch::end()
    {
        if (!this->_has_begun)
        {
            throw "You have to call 'begin()' on a SpriteBatch first.";
        }
        flush();

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

        int textureIndex = -1;
        for (int i = 0; i < this->_num_active_textures; i++)
        {
            if (texture == this->_activeTextures[i])
                textureIndex = i;
        }
        if (textureIndex < 0)
        {
            if (this->_num_active_textures == NUM_SIMULT_TEXTURES)
                // In this case we need to flush, as this texture does not fit anymore!
                flush();

            textureIndex = _num_active_textures;
            this->_num_active_textures++;
            this->_activeTextures[textureIndex] = texture;
        }
        this->add_sprite_data(texture, destination_rectangle, source_rectangle, color, z);
        this->_num_active_sprites++;

        if (this->_num_active_sprites > MAX_BATCH_SIZE)
            this->flush();
    }

    void SpriteBatch::add_sprite_data(
        const Texture2D &texture,
        const Rectangle &destination_rectangle,
        const Rectangle *const source_rectangle,
        const Color &color,
        float z)
    {
        glm::vec2 posTopLeft(destination_rectangle.x, destination_rectangle.y);
        glm::vec2 posTopRight(destination_rectangle.x + destination_rectangle.width, destination_rectangle.y);
        glm::vec2 posBottomLeft(destination_rectangle.x, destination_rectangle.y + destination_rectangle.height);
        glm::vec2 posBottomRight(destination_rectangle.x + destination_rectangle.width, destination_rectangle.y + destination_rectangle.height);

        glm::vec2 uvTopLeft(0.0f, 0.0f);
        glm::vec2 uvTopRight(1.0f, 0.0f);
        glm::vec2 uvBottomLeft(0.0f, 1.0f);
        glm::vec2 uvBottomRight(1.0f, 1.0f);
        if (source_rectangle)
        {
            double textureWidth = texture.get_width();
            double textureHeight = texture.get_height();

            uvTopLeft.x = (GLfloat)source_rectangle->x / textureWidth;
            uvTopLeft.y = (GLfloat)source_rectangle->y / textureHeight;

            uvTopRight.x = (GLfloat)(source_rectangle->x + source_rectangle->width) / textureWidth;
            uvTopRight.y = (GLfloat)source_rectangle->y / textureHeight;

            uvBottomLeft.x = (GLfloat)source_rectangle->x / textureWidth;
            uvBottomLeft.y = (GLfloat)(source_rectangle->y + source_rectangle->height) / textureHeight;

            uvBottomRight.x = (GLfloat)(source_rectangle->x + source_rectangle->width) / textureWidth;
            uvBottomRight.y = (GLfloat)(source_rectangle->y + source_rectangle->height) / textureHeight;
        }

        int offset_vbo = _num_active_sprites * SPRITE_SIZE_VBO;
        if (offset_vbo >= _sprite_data_vbo.size())
            _sprite_data_vbo.resize((_num_active_sprites + 1) * SPRITE_SIZE_VBO);

        int offset_ebo = _num_active_sprites * SPRITE_SIZE_EBO;
        if (offset_ebo >= _sprite_data_ebo.size())
            _sprite_data_ebo.resize((_num_active_sprites + 1) * SPRITE_SIZE_EBO);

        int offset_vertices = _num_active_sprites * 4;

        // VBO
        // Top Right
        _sprite_data_vbo[offset_vbo + 0] = posTopRight.x;
        _sprite_data_vbo[offset_vbo + 1] = posTopRight.y;
        //_sprite_data_vbo[offset_vbo + 2] = z;
        _sprite_data_vbo[offset_vbo + 2] = uvTopRight.x;
        _sprite_data_vbo[offset_vbo + 3] = uvTopRight.y;

        //  Bottom Right
        _sprite_data_vbo[offset_vbo + 4] = posBottomRight.x;
        _sprite_data_vbo[offset_vbo + 5] = posBottomRight.y;
        //_sprite_data_vbo[offset_vbo + 7] = z;
        _sprite_data_vbo[offset_vbo + 6] = uvBottomRight.x;
        _sprite_data_vbo[offset_vbo + 7] = uvBottomRight.y;

        // Bottom Left
        _sprite_data_vbo[offset_vbo + 8] = posBottomLeft.x;
        _sprite_data_vbo[offset_vbo + 9] = posBottomLeft.y;
        //_sprite_data_vbo[offset_vbo + 12] = z;
        _sprite_data_vbo[offset_vbo + 10] = uvBottomLeft.x;
        _sprite_data_vbo[offset_vbo + 11] = uvBottomLeft.y;

        // Top Left
        _sprite_data_vbo[offset_vbo + 12] = posTopLeft.x;
        _sprite_data_vbo[offset_vbo + 13] = posTopLeft.y;
        //_sprite_data_vbo[offset_vbo + 17] = z;
        _sprite_data_vbo[offset_vbo + 14] = uvTopLeft.x;
        _sprite_data_vbo[offset_vbo + 15] = uvTopLeft.y;

        // EBO
        _sprite_data_ebo[offset_ebo] = offset_vertices;
        _sprite_data_ebo[offset_ebo + 1] = offset_vertices + 1;
        _sprite_data_ebo[offset_ebo + 2] = offset_vertices + 3;
        _sprite_data_ebo[offset_ebo + 3] = offset_vertices + 1;
        _sprite_data_ebo[offset_ebo + 4] = offset_vertices + 2;
        _sprite_data_ebo[offset_ebo + 5] = offset_vertices + 3;
    }

    void SpriteBatch::flush()
    {
        glBindVertexArray(_vao);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, _num_active_sprites * SPRITE_SIZE_VBO * sizeof(float), &_sprite_data_vbo[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _num_active_sprites * SPRITE_SIZE_EBO * sizeof(unsigned int), &_sprite_data_ebo[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        _shader.use();
        for (int i = 0; i < this->_num_active_textures; i++)
            _activeTextures[i].use(i);

        _shader.set_int("Texture", 0);
        _shader.set_mat4("WVP", this->_wvp);

        // glDrawArrays(GL_TRIANGLES, 0, _num_active_sprites * 6);
        glDrawElements(GL_TRIANGLES, _num_active_sprites * SPRITE_SIZE_EBO, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        _num_active_sprites = 0;
        _num_active_textures = 0;
    }

    const std::string SpriteBatch::VERTEX_SHADER_SOURCE = R"(
    #version 330 core
    layout (location = 0) in vec4 vertex;
    
    out vec2 TexCoord;
    
    uniform mat4 WVP;
    
    void main()
    {
        gl_Position = WVP * vec4(vertex.xy, 0.0, 1.0);
        TexCoord = vertex.zw;
    })";

    const std::string SpriteBatch::FRAGMENT_SHADER_SOURCE = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec2 TexCoord;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec4 color = texture(Texture, TexCoord);
        FragColor = color;
    })";
} // namespace engine