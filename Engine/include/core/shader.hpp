#ifndef __ENGINE_SHADER_H__
#define __ENGINE_SHADER_H__

#include <string>

#include "glad/glad.h"
#include "glm/glm.hpp"

namespace engine
{
    class Shader
    {
    private:
        GLuint glShaderProgram = 0;

        int checkCompileErrors(GLuint shader, std::string type) const;
        GLuint compileShader(const std::string source, const std::string name, const GLenum shaderType) const;
        GLuint compileProgram(const GLuint vertex, const GLuint geometry, const GLuint fragment);
        std::string loadShaderSource(const std::string path) const;

    public:
        Shader();
        ~Shader();

        int loadShaderFromFile(const std::string vertexPath, const std::string geometryPath, const std::string fragmentPath);
        int loadShaderFromSource(const std::string vertexSource, const std::string geometrySource, const std::string fragmentSource);
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

        /*
            @return a GLuint object that represents the shader program
        */
        GLuint getglShaderProgram() const;
    };
} // namespace engine

#endif