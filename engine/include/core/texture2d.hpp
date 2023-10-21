#pragma once

#include "glad/glad.h"

#include <string>

#include "resource.hpp"

namespace engine
{
    class Texture2D : public Resource
    {
    private:
        GLuint _gl_texture = 0;
        GLint _internalFormat;
        GLint _imageFormat;
        GLint _wrapS;
        GLint _wrapT;
        GLint _filterMin;
        GLint _filterMax;
        int _width;
        int _height;

    public:
        Texture2D();

        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        void create_texture_from_raw_data(int width, int height, unsigned char *data);
        /*
            Activate the texture in openGL at the specified unit
        */
        void use(GLenum unit) const;

        /*
            @return a GLuint object that represents the texture
        */
        GLuint get_gl_texture() const { return _gl_texture; }
        int get_width() const { return _width; }
        int get_height() const { return _height; }
        void set_internal_format(GLint interal_format);
        void set_image_format(GLint image_format);

        friend bool operator==(const Texture2D &lhs, const Texture2D &rhs) { return lhs._gl_texture == rhs._gl_texture; }
        friend bool operator!=(const Texture2D &lhs, const Texture2D &rhs) { return !(lhs == rhs); }
    };
} // namespace engine
