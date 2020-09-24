#include "core/texture2D.hpp"

#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "core/stb_image.hpp"
#include "core/log.hpp"

namespace engine
{
    Texture2D::Texture2D() : internalFormat(GL_RGB),
                             imageFormat(GL_RGB),
                             wrapS(GL_REPEAT),
                             wrapT(GL_REPEAT),
                             filterMin(GL_LINEAR),
                             filterMax(GL_LINEAR)
    {
    }

    void Texture2D::unloadResource()
    {
        if (this->glTexture != 0)
        {
            glDeleteTextures(1, &this->glTexture);
            this->glTexture = 0;
        }
    }

    bool Texture2D::loadResource(ResourceManager &resourceManager, const std::string &filename, va_list args)
    {
        bool hasAlphaChannel = va_arg(args, int);
        if (hasAlphaChannel)
        {
            this->setInternalFormat(GL_RGBA);
            this->setImageFormat(GL_RGBA);
        }
        // load image
        int width, height, nrChannels;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
        // now generate texture
        if (data)
        {
            this->createTextureFromRawData(width, height, data);
            // and finally free image data
            stbi_image_free(data);
        }
        else
        {
            std::stringstream error;
            error << "Failed to load texture. FILE: " << filename;
            Log::e(error.str());
            return false;
        }

        return true;
    }

    void Texture2D::createTextureFromRawData(const int width, const int height, unsigned char *data)
    {
        this->width = width;
        this->height = height;

        glGenTextures(1, &glTexture);
        glBindTexture(GL_TEXTURE_2D, glTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, this->width, this->height, 0, this->imageFormat, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMax);

        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture2D::use(const unsigned char unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, glTexture);
    }

    GLuint Texture2D::getglTexture() const { return this->glTexture; }
    int Texture2D::getWidth() const { return this->width; }
    int Texture2D::getHeight() const { return this->height; }

    void Texture2D::setInternalFormat(const GLuint internalFormat) { this->internalFormat = internalFormat; }
    void Texture2D::setImageFormat(const GLuint imageFormat) { this->imageFormat = imageFormat; }
} // namespace engine