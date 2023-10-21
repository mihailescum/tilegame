#pragma once

#include "glad/glad.h"

#include <string>

#include "resource.hpp"

namespace engine
{
    class Texture2D : public Resource
    {
    private:
        GLuint glTexture = 0;
        GLint internalFormat;
        GLint imageFormat;
        GLint wrapS;
        GLint wrapT;
        GLint filterMin;
        GLint filterMax;
        int width;
        int height;

    public:
        Texture2D();

        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override;

        void createTextureFromRawData(int width, int height, unsigned char *data);
        /*
            Activate the texture in openGL at the specified unit
        */
        void use(GLenum unit) const;

        /*
            @return a GLuint object that represents the texture
        */
        GLuint getglTexture() const { return glTexture; }
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        void setInternalFormat(GLint internalFormat);
        void setImageFormat(GLint imageFormat);

        friend bool operator==(const Texture2D &lhs, const Texture2D &rhs) { return lhs.glTexture == rhs.glTexture; }
        friend bool operator!=(const Texture2D &lhs, const Texture2D &rhs) { return !(lhs == rhs); }
    };
} // namespace engine
