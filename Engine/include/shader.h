#ifndef __ENGINE_SHADER_H__
#define __ENGINE_SHADER_H__

#include <string>

#include "glad/glad.h"
#include "glm/glm.hpp"

namespace engine
{
    class Shader
    {
    public:
        Shader();
        ~Shader();
        /*
            @return a GLuint object that represents the shader program
        */
        GLuint getglShaderProgram() const;
        int loadShader(const std::string vertexPath, const std::string geometryPath, const std::string fragmentPath);
        void setBool(const std::string &name, GLboolean value) const;
        void setInt(const std::string &name, GLint value) const;
        void setFloat(const std::string &name, GLfloat value) const;
        void setVec4f(const std::string &name, glm::vec4 value) const;
        void setMatrix4fv(const std::string &name, glm::mat4 value) const;
        /*
            Activate the texture in openGL
        */
        void use() const;
        void deleteShader();

    private:
        GLuint glShaderProgram = 0;
        int checkCompileErrors(GLuint shader, std::string type) const;
        GLuint loadAndCompileShader(const std::string path, const std::string name, const GLenum shaderType);
    };
} // namespace engine

#endif