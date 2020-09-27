#pragma once

#include "glad/glad.h"

#include <string>

#include "core/resource.hpp"

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
        int width;
        int height;

    public:
        Texture2D();

        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override;

        void createTextureFromRawData(const int width, const int height, unsigned char *data);
        /*
            Activate the texture in openGL at the specified unit
        */
        void use(const unsigned char unit) const;

        /*
            @return a GLuint object that represents the texture
        */
        GLuint getglTexture() const;
        int getWidth() const;
        int getHeight() const;
        void setInternalFormat(const GLuint internalFormat);
        void setImageFormat(const GLuint imageFormat);

        friend bool operator==(const Texture2D &lhs, const Texture2D &rhs) { return lhs.glTexture == rhs.glTexture; }
        friend bool operator!=(const Texture2D &lhs, const Texture2D &rhs) { return !(lhs == rhs); }
    };
} // namespace engine
