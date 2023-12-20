#include "core/texture2d.hpp"

#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.hpp"
#include "core/log.hpp"

#include <iostream>

namespace engine
{
    Texture2D::Texture2D() : _internalFormat(GL_RGB),
                             _imageFormat(GL_RGB),
                             _wrapS(GL_REPEAT),
                             _wrapT(GL_REPEAT),
                             _filterMin(GL_LINEAR),
                             _filterMax(GL_LINEAR)
    {
    }

    void Texture2D::unload_resource()
    {
        if (this->_gl_texture != 0)
        {
            glDeleteTextures(1, &this->_gl_texture);
            this->_gl_texture = 0;
        }
    }

    bool Texture2D::load_resource(ResourceManager &_resource_manager, va_list args)
    {
        bool hasAlphaChannel = va_arg(args, int);
        if (hasAlphaChannel)
        {
            this->internal_format(GL_RGBA);
            this->image_format(GL_RGBA);
        }
        // load image
        int width, height, nrChannels;
        unsigned char *data = stbi_load(this->_resource_path.c_str(), &width, &height, &nrChannels, 0);
        // now generate texture
        if (data)
        {

            this->create_texture_from_raw_data(width, height, data);
            // and finally free image data
            stbi_image_free(data);
        }
        else
        {
            Log::e("Failed to load texture. FILE: ", this->_resource_path);
            return false;
        }

        return true;
    }

    void Texture2D::create_texture_from_raw_data(int width, int height, unsigned char *data)
    {
        this->_width = width;
        this->_height = height;

        glGenTextures(1, &_gl_texture);
        glBindTexture(GL_TEXTURE_2D, _gl_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, this->_internalFormat, this->_width, this->_height, 0, this->_imageFormat, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->_wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->_wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->_filterMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->_filterMax);

        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture2D::use(GLenum unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, _gl_texture);
    }

    void Texture2D::internal_format(GLint internalFormat) { this->_internalFormat = internalFormat; }
    void Texture2D::image_format(GLint imageFormat) { this->_imageFormat = imageFormat; }
} // namespace engine