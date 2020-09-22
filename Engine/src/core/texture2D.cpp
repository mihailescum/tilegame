#include "core/texture2D.hpp"

#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "core/stb_image.hpp"
#include "core/log.hpp"

namespace engine
{
    Texture2D::~Texture2D() {
        this->deleteTexture();
    }

    void Texture2D::createTextureFromRawData(const int width, const int height, const GLenum colorFormat, const GLenum dataType, const bool generateMipmap, const void *data)
    {
        glGenTextures(1, &glTexture);
        glBindTexture(GL_TEXTURE_2D, glTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat, dataType, data);
        if (generateMipmap)
            glGenerateMipmap(GL_TEXTURE_2D);
    }

    int Texture2D::loadTexture(const std::string path, const GLenum colorFormat, const bool flipVertically)
    {
        int nrChannels;
        stbi_set_flip_vertically_on_load(flipVertically);
        unsigned char *data = stbi_load(path.c_str(), &this->width, &this->height, &nrChannels, 0);
        if (data)
        {
            createTextureFromRawData(width, height, colorFormat, GL_UNSIGNED_BYTE, true, data);
        }
        else
        {
            std::stringstream error;
            error << "Failed to load texture. FILE: " << path;
            Log::e(error.str());
            return 0;
        }

        stbi_image_free(data);
        return 1;
    }

    void Texture2D::deleteTexture()
    {
        if (glTexture != 0)
            glDeleteTextures(1, &glTexture);
    }

    void Texture2D::use(const unsigned char unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, glTexture);
    }

    GLuint Texture2D::getglTexture() const
    {
        return this->glTexture;
    }

    int Texture2D::getWidth() const
    {
        return this->width;
    }

    int Texture2D::getHeight() const
    {
        return this->height;
    }
} // namespace engine