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
        if (this->_vao != 0)
            glDeleteVertexArrays(1, &_vao);
    }

    void SpriteBatch::create()
    {
        glGenBuffers(1, &this->_vbo);
        glGenVertexArrays(1, &this->_vao);

        _shader.compile(SpriteBatch::VERTEX_SHADER_SOURCE, "", SpriteBatch::FRAGMENT_SHADER_SOURCE);
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

        const Viewport &viewport = this->_graphics_device.get_viewport();
        glm::mat4 projection = glm::ortho(
            static_cast<float>(viewport.x),
            static_cast<float>(viewport.width),
            static_cast<float>(viewport.height),
            static_cast<float>(viewport.y),
            -1.0f,
            1.0f);

        this->_wvp = projection * transform;

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

    void SpriteBatch::draw(const Texture2D &texture, const Rectangle &destinationRectangle, const Color &color)
    {
        this->draw(texture, destinationRectangle, nullptr, color);
    }

    void SpriteBatch::draw(const Texture2D &texture, const Rectangle &destinationRectangle, const Rectangle *sourceRectangle, const Color &color)
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
        this->add_sprite_data(texture, destinationRectangle, sourceRectangle);

        this->_num_active_sprites++;
        if (this->_num_active_sprites > MAX_BATCH_SIZE)
            this->flush();
    }

    void SpriteBatch::add_sprite_data(const Texture2D &texture, const Rectangle &destinationRectangle, const Rectangle *sourceRectangle)
    {
        glm::vec2 posTopLeft(destinationRectangle.x, destinationRectangle.y);
        glm::vec2 posTopRight(destinationRectangle.x + destinationRectangle.width, destinationRectangle.y);
        glm::vec2 posBottomLeft(destinationRectangle.x, destinationRectangle.y + destinationRectangle.height);
        glm::vec2 posBottomRight(destinationRectangle.x + destinationRectangle.width, destinationRectangle.y + destinationRectangle.height);

        glm::vec2 uvTopLeft(0.0f, 0.0f);
        glm::vec2 uvTopRight(1.0f, 0.0f);
        glm::vec2 uvBottomLeft(0.0f, 1.0f);
        glm::vec2 uvBottomRight(1.0f, 1.0f);
        if (sourceRectangle != nullptr)
        {
            double textureWidth = texture.get_width();
            double textureHeight = texture.get_height();

            uvTopLeft.x = (GLfloat)sourceRectangle->x / textureWidth;
            uvTopLeft.y = (GLfloat)sourceRectangle->y / textureHeight;

            uvTopRight.x = (GLfloat)(sourceRectangle->x + sourceRectangle->width) / textureWidth;
            uvTopRight.y = (GLfloat)sourceRectangle->y / textureHeight;

            uvBottomLeft.x = (GLfloat)sourceRectangle->x / textureWidth;
            uvBottomLeft.y = (GLfloat)(sourceRectangle->y + sourceRectangle->height) / textureHeight;

            uvBottomRight.x = (GLfloat)(sourceRectangle->x + sourceRectangle->width) / textureWidth;
            uvBottomRight.y = (GLfloat)(sourceRectangle->y + sourceRectangle->height) / textureHeight;
        }

        const int sizeOfSprite = 24;
        int offset = this->_num_active_sprites * sizeOfSprite;
        if (offset >= this->_sprite_data.size())
            _sprite_data.resize((this->_num_active_sprites + 1) * sizeOfSprite);

        // Bottom Left
        this->_sprite_data[offset] = posBottomLeft.x;
        this->_sprite_data[offset + 1] = posBottomLeft.y;
        this->_sprite_data[offset + 2] = uvBottomLeft.x;
        this->_sprite_data[offset + 3] = uvBottomLeft.y;
        // Top Right
        this->_sprite_data[offset + 4] = posTopRight.x;
        this->_sprite_data[offset + 5] = posTopRight.y;
        this->_sprite_data[offset + 6] = uvTopRight.x;
        this->_sprite_data[offset + 7] = uvTopRight.y;
        // Top Left
        this->_sprite_data[offset + 8] = posTopLeft.x;
        this->_sprite_data[offset + 9] = posTopLeft.y;
        this->_sprite_data[offset + 10] = uvTopLeft.x;
        this->_sprite_data[offset + 11] = uvTopLeft.y;
        // Bottom Left
        this->_sprite_data[offset + 12] = posBottomLeft.x;
        this->_sprite_data[offset + 13] = posBottomLeft.y;
        this->_sprite_data[offset + 14] = uvBottomLeft.x;
        this->_sprite_data[offset + 15] = uvBottomLeft.y;
        // Bottom Right
        this->_sprite_data[offset + 16] = posBottomRight.x;
        this->_sprite_data[offset + 17] = posBottomRight.y;
        this->_sprite_data[offset + 18] = uvBottomRight.x;
        this->_sprite_data[offset + 19] = uvBottomRight.y;
        // Top Right
        this->_sprite_data[offset + 20] = posTopRight.x;
        this->_sprite_data[offset + 21] = posTopRight.y;
        this->_sprite_data[offset + 22] = uvTopRight.x;
        this->_sprite_data[offset + 23] = uvTopRight.y;
    }

    void SpriteBatch::flush()
    {
        glBindVertexArray(this->_vao);

        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, _num_active_sprites * 24 * sizeof(float), &_sprite_data[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        _shader.use();
        for (int i = 0; i < this->_num_active_textures; i++)
            _activeTextures[i].use(i);

        _shader.set_int("Texture", 0);
        _shader.set_mat4("WVP", this->_wvp);

        glDrawArrays(GL_TRIANGLES, 0, _num_active_sprites * 6);

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