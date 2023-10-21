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
        GLuint internalFormat;
        GLuint imageFormat;
        GLuint wrapS;
        GLuint wrapT;
        GLuint filterMin;
        GLuint filterMax;
        GLuint width;
        GLuint height;

    public:
        Texture2D();

        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override;

        void createTextureFromRawData(const GLuint width, const GLuint height, unsigned char *data);
        /*
            Activate the texture in openGL at the specified unit
        */
        void use(const GLubyte unit) const;

        /*
            @return a GLuint object that represents the texture
        */
        GLuint getglTexture() const { return glTexture; }
        GLuint getWidth() const { return width; }
        GLuint getHeight() const { return height; }
        void setInternalFormat(const GLuint internalFormat);
        void setImageFormat(const GLuint imageFormat);

        friend bool operator==(const Texture2D &lhs, const Texture2D &rhs) { return lhs.glTexture == rhs.glTexture; }
        friend bool operator!=(const Texture2D &lhs, const Texture2D &rhs) { return !(lhs == rhs); }
    };
} // namespace engine
