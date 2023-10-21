#include "glad/glad.h"

#include <glm/gtc/matrix_transform.hpp>

#include "core/spritebatch.hpp"

namespace engine
{
    SpriteBatch::SpriteBatch(GraphicsDevice &graphicsDevice)
        : graphicsDevice(graphicsDevice),
          num_active_textures(0),
          num_active_sprites(0),
          begun(false)
    {
    }

    SpriteBatch::~SpriteBatch()
    {
        if (this->VBO != 0)
            glDeleteBuffers(1, &VBO);
        if (this->VAO != 0)
            glDeleteVertexArrays(1, &VAO);
    }

    void SpriteBatch::create()
    {
        glGenBuffers(1, &this->VBO);
        glGenVertexArrays(1, &this->VAO);

        shader.compile(SpriteBatch::vertexShaderSource, "", SpriteBatch::fragmentShaderSource);
    }

    void SpriteBatch::begin(const bool alphaBlendingEnabled)
    {
        glm::mat4 transform(1.0);
        this->begin(transform, alphaBlendingEnabled);
    }

    void SpriteBatch::begin(const glm::mat4 &transform, const bool alphaBlendingEnabled)
    {
        if (this->begun)
        {
            throw "'begin()' was already called. Call 'end()' first.";
        }
        this->begun = true;

        const Viewport &viewport = this->graphicsDevice.getViewport();
        glm::mat4 projection = glm::ortho(
            static_cast<float>(viewport.x),
            static_cast<float>(viewport.width),
            static_cast<float>(viewport.height),
            static_cast<float>(viewport.y),
            -1.0f,
            1.0f);

        this->wvp = projection * transform;

        if (alphaBlendingEnabled)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }

    void SpriteBatch::end()
    {
        if (!this->begun)
        {
            throw "You have to call 'begin()' on a SpriteBatch first.";
        }
        flush();

        glDisable(GL_BLEND);

        this->begun = false;
    }

    void SpriteBatch::draw(const Texture2D &texture, const Rectangle &destinationRectangle, const Color &color)
    {
        this->draw(texture, destinationRectangle, nullptr, color);
    }

    void SpriteBatch::draw(const Texture2D &texture, const Rectangle &destinationRectangle, const Rectangle *sourceRectangle, const Color &color)
    {
        if (!this->begun)
        {
            throw "You have to call 'begin()' on a SpriteBatch first.";
        }

        int textureIndex = -1;
        for (int i = 0; i < this->num_active_textures; i++)
        {
            if (texture == this->activeTextures[i])
                textureIndex = i;
        }
        if (textureIndex < 0)
        {
            if (this->num_active_textures == NUM_SIMULT_TEXTURES)
                // In this case we need to flush, as this texture does not fit anymore!
                flush();

            textureIndex = num_active_textures;
            this->num_active_textures++;
            this->activeTextures[textureIndex] = texture;
        }
        this->addSpriteData(texture, destinationRectangle, sourceRectangle);

        this->num_active_sprites++;
        if (this->num_active_sprites > MAX_BATCH_SIZE)
            this->flush();
    }

    void SpriteBatch::addSpriteData(const Texture2D &texture, const Rectangle &destinationRectangle, const Rectangle *sourceRectangle)
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
            double textureWidth = texture.getWidth();
            double textureHeight = texture.getHeight();

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
        int offset = this->num_active_sprites * sizeOfSprite;
        if (offset >= this->spriteData.size())
            spriteData.resize((this->num_active_sprites + 1) * sizeOfSprite);

        // Bottom Left
        this->spriteData[offset] = posBottomLeft.x;
        this->spriteData[offset + 1] = posBottomLeft.y;
        this->spriteData[offset + 2] = uvBottomLeft.x;
        this->spriteData[offset + 3] = uvBottomLeft.y;
        // Top Right
        this->spriteData[offset + 4] = posTopRight.x;
        this->spriteData[offset + 5] = posTopRight.y;
        this->spriteData[offset + 6] = uvTopRight.x;
        this->spriteData[offset + 7] = uvTopRight.y;
        // Top Left
        this->spriteData[offset + 8] = posTopLeft.x;
        this->spriteData[offset + 9] = posTopLeft.y;
        this->spriteData[offset + 10] = uvTopLeft.x;
        this->spriteData[offset + 11] = uvTopLeft.y;
        // Bottom Left
        this->spriteData[offset + 12] = posBottomLeft.x;
        this->spriteData[offset + 13] = posBottomLeft.y;
        this->spriteData[offset + 14] = uvBottomLeft.x;
        this->spriteData[offset + 15] = uvBottomLeft.y;
        // Bottom Right
        this->spriteData[offset + 16] = posBottomRight.x;
        this->spriteData[offset + 17] = posBottomRight.y;
        this->spriteData[offset + 18] = uvBottomRight.x;
        this->spriteData[offset + 19] = uvBottomRight.y;
        // Top Right
        this->spriteData[offset + 20] = posTopRight.x;
        this->spriteData[offset + 21] = posTopRight.y;
        this->spriteData[offset + 22] = uvTopRight.x;
        this->spriteData[offset + 23] = uvTopRight.y;
    }

    void SpriteBatch::flush()
    {
        glBindVertexArray(this->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, num_active_sprites * 24 * sizeof(float), &spriteData[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        shader.use();
        for (int i = 0; i < this->num_active_textures; i++)
            activeTextures[i].use(i);

        shader.setInt("Texture", 0);
        shader.setMatrix4fv("WVP", this->wvp);

        glDrawArrays(GL_TRIANGLES, 0, num_active_sprites * 6);

        num_active_sprites = 0;
        num_active_textures = 0;
    }

    const std::string SpriteBatch::vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec4 vertex;
    
    out vec2 TexCoord;
    
    uniform mat4 WVP;
    
    void main()
    {
        gl_Position = WVP * vec4(vertex.xy, 0.0, 1.0);
        TexCoord = vertex.zw;
    })";

    const std::string SpriteBatch::fragmentShaderSource = R"(
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