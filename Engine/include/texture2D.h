#ifndef __ENGINE_TEXTURE2D_H__
#define __ENGINE_TEXTURE2D_H__

#include <glad/glad.h>

#include <string>

namespace engine
{
    class Texture2D
    {
    public:
        int loadTexture(const std::string path, const GLenum colorFormat, const bool flipVertically);
        void createTextureFromRawData(const int width, const int height, const GLenum colorFormat, const GLenum dataType, const bool generateMipmap, const void *data);
        void deleteTexture();

        /*
            Activate the texture in openGL at the specified unit
        */
        void use(const unsigned char unit) const;

        /*
            @return a GLuint object that represents the texture
        */
        GLuint getglTexture() const;

    private:
        GLuint glTexture = 0;
    };
} // namespace engine

#endif