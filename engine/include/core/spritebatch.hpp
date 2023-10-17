#pragma once

#include <vector>
#include <memory>

#include "core/graphicsdevice.hpp"
#include "core/texture2D.hpp"
#include "core/shader.hpp"

namespace engine
{
    class SpriteBatch
    {
    private:
        static const int NUM_SIMULT_TEXTURES = 1;
        static const int MAX_BATCH_SIZE = 4096;

        static const std::string vertexShaderSource;
        static const std::string fragmentShaderSource;

        GraphicsDevice &graphicsDevice;
        GLuint VBO = 0;
        GLuint VAO = 0;

        Shader shader;

        glm::mat4 wvp;
        Texture2D activeTextures[NUM_SIMULT_TEXTURES];
        std::vector<float> spriteData;
        int num_active_textures;
        int num_active_sprites;
        bool begun;

        void flush();
        void addSpriteData(const Texture2D &texture, const Rectangle &destinationRectangle, const Rectangle *sourceRectangle);

    public:
        SpriteBatch(GraphicsDevice &graphicsDevice);
        SpriteBatch(const SpriteBatch &spriteBatch) = delete;
        virtual ~SpriteBatch();

        void create();
        void begin(const bool alphaBlendingEnabled);
        void begin(const glm::mat4 &transform, const bool alphaBlendingEnabled);
        void draw(const Texture2D &texture, const Rectangle &destinationRectangle, const Color &color);
        void draw(const Texture2D &texture, const Rectangle &destinationRectangle, const Rectangle *sourceRectangle, const Color &color);
        void end();
    };
} // namespace engine
