#include "core/texture2d.hpp"

#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.hpp"
#include "core/log.hpp"

#include <iostream>

namespace engine
{
    Texture2D::Texture2D() : Texture2D(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR)
    {
    }

    Texture2D::Texture2D(GLint internal_format, GLenum image_format, GLenum image_type, GLint wrap_s, GLint wrap_t, GLint filter_min, GLint filter_max)
        : _gl_texture(0),
          _internal_format(internal_format),
          _image_format(image_format),
          _image_type(image_type),
          _wrap_s(wrap_s),
          _wrap_t(wrap_t),
          _filter_min(filter_min),
          _filter_max(filter_max)

    {
    }

    void Texture2D::unload_resource()
    {
        if (_gl_texture != 0)
        {
            glDeleteTextures(1, &_gl_texture);
            _gl_texture = 0;
        }
    }

    bool Texture2D::load_resource(ResourceManager &_resource_manager, va_list args)
    {
        bool hasAlphaChannel = va_arg(args, int);
        if (hasAlphaChannel)
        {
            internal_format(GL_RGBA);
            image_format(GL_RGBA);
        }
        // load image
        int width, height, nrChannels;
        unsigned char *data = stbi_load(_resource_path.c_str(), &width, &height, &nrChannels, 0);
        // now generate texture
        if (data)
        {
            create_texture_from_raw_data(width, height, data);
            // and finally free image data
            stbi_image_free(data);
        }
        else
        {
            Log::e("Failed to load texture. FILE: ", _resource_path);
            return false;
        }

        return true;
    }

    void Texture2D::create_texture_from_raw_data(int width, int height, unsigned char *data)
    {
        _dimensions = glm::ivec2(width, height);

        glGenTextures(1, &_gl_texture);
        glBindTexture(GL_TEXTURE_2D, _gl_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, _internal_format, width, height, 0, _image_format, _image_type, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filter_min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filter_max);

        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture2D::use(GLenum unit) const
    {
        use(_gl_texture, unit);
    }

    void Texture2D::use(native_type gl_texture, GLenum unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, gl_texture);
    }

    void Texture2D::internal_format(GLint internalFormat) { _internal_format = internalFormat; }
    void Texture2D::image_format(GLint imageFormat) { _image_format = imageFormat; }
} // namespace engine