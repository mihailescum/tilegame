#ifndef __ENGINE_SPRITEBATCH_H__
#define __ENGINE_SPRITEBATCH_H__

#include "graphicsdevice.hpp"
#include "texture2D.hpp"
#include "shader.hpp"

#include <vector>

namespace engine
{
    class SpriteBatch
    {
    private:
        static const int NUM_SIMULT_TEXTURES = 1;

        const GraphicsDevice &graphicsDevice;
        Texture2D activeTextures[NUM_SIMULT_TEXTURES];
        int num_active_textures;
        int num_active_sprites;
        bool begun;
        void flush();
        void addSpriteData(const Texture2D &texture, const Rectangle& destinationRectangle, const Rectangle *sourceRectangle);

        std::vector<float> spriteData;

        GLuint VBO = 0;
        GLuint VAO = 0;
        Shader shader;
        glm::mat4 wvp;

        static const std::string vertexShaderSource;
        static const std::string fragmentShaderSource;

    public:
        SpriteBatch(const GraphicsDevice &graphicsDevice);
        ~SpriteBatch();
        void create();
        void begin();
        void begin(const glm::mat4 &transform);
        void draw(const Texture2D &texture, const Rectangle &destinationRectangle, const Rectangle *sourceRectangle, const Color &color);
        void draw(const Texture2D &texture, const Rectangle &destinationRectangle, const Color &color);
        void end();
    };
} // namespace engine

#endif