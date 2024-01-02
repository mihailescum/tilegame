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
        GLuint _gl_texture;
        GLint _internalFormat;
        GLint _imageFormat;
        GLint _wrapS;
        GLint _wrapT;
        GLint _filterMin;
        GLint _filterMax;
        glm::ivec2 _size;

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
        // TODO check if this is necessary
        GLuint gl_texture() const { return _gl_texture; }
        const glm::ivec2 &size() const { return _size; }
        void internal_format(GLint interal_format);
        void image_format(GLint image_format);

        friend bool operator==(const Texture2D &lhs, const Texture2D &rhs) { return lhs._gl_texture == rhs._gl_texture; }
        friend bool operator!=(const Texture2D &lhs, const Texture2D &rhs) { return !(lhs == rhs); }
    };
} // namespace engine
