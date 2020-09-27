#pragma once

#include <string>

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "core/resource.hpp"

namespace engine
{
    class Shader : public Resource
    {
    private:
        GLuint glShaderProgram = 0;

        // Returns true if there were errors
        bool checkCompileErrors(GLuint shader, std::string type) const;
        GLuint compileShader(const std::string &source, const std::string &name, const GLenum shaderType) const;
        // Returns true if compilation was successful
        bool compileProgram(const GLuint vertex, const GLuint geometry, const GLuint fragment);

        std::string loadShaderSource(const std::string &path) const;

    public:
        Shader();

        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override;

        void setBool(const std::string &name, GLboolean value) const;
        void setInt(const std::string &name, GLint value) const;
        void setFloat(const std::string &name, GLfloat value) const;
        void setVec4f(const std::string &name, glm::vec4 value) const;
        void setMatrix4fv(const std::string &name, glm::mat4 value) const;

        bool compile(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource);
        void use() const;

        /*
            @return a GLuint object that represents the shader program
        */
        GLuint getglShaderProgram() const;
    };
} // namespace engine
