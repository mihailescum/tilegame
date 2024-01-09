#pragma once

#include <string>

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "resource.hpp"
#include "log.hpp"

namespace engine
{
    class Texture2D : public Resource
    {
    private:
        typedef GLuint _native_type;
        _native_type _gl_texture;
        GLint _internalFormat;
        GLint _imageFormat;
        GLint _wrapS;
        GLint _wrapT;
        GLint _filterMin;
        GLint _filterMax;
        glm::ivec2 _dimensions;

    public:
        typedef _native_type native_type;

        Texture2D();

        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        void create_texture_from_raw_data(int width, int height, unsigned char *data);

        /*
            Activate the texture in openGL at the specified unit
        */
        void use(GLenum unit = 0) const;
        static void use(native_type gl_texture, GLenum unit);
        const glm::ivec2 &dimensions() const { return _dimensions; }
        void internal_format(GLint interal_format);
        void image_format(GLint image_format);

        operator native_type() const { return _gl_texture; }
        friend bool operator==(const Texture2D &lhs, const Texture2D &rhs) { return lhs._gl_texture == rhs._gl_texture; }
        friend bool operator!=(const Texture2D &lhs, const Texture2D &rhs) { return !(lhs == rhs); }
    };
} // namespace engine
