#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>

#include "log.h"
#include "glm/ext.hpp"

namespace engine
{
    Shader::Shader()
    {
        glShaderProgram = 0;
    }

    Shader::~Shader()
    {
    }

    void Shader::deleteShader()
    {
        if (glShaderProgram != 0)
            glDeleteProgram(glShaderProgram);
    }

    GLuint Shader::loadAndCompileShader(const std::string path, const std::string name, const GLenum shaderType)
    {
        std::string code;
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            file.open(path);
            std::stringstream codeStream;
            codeStream << file.rdbuf();
            file.close();
            code = codeStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::stringstream error;
            error << "SHADER::FILE_NOT_SUCCESFULLY_READ. FILE: " << path;
            Log::e(error.str());
            return -1;
        }
        const char *shaderCode = code.c_str();
        GLuint result;
        result = glCreateShader(shaderType);
        glShaderSource(result, 1, &shaderCode, NULL);
        glCompileShader(result);
        if (!checkCompileErrors(result, name))
            return -1;
        else
            return result;
    }

    int Shader::loadShader(const std::string vertexPath, const std::string geometryPath, const std::string fragmentPath)
    {
        GLuint vertex, geometry, fragment;
        // compile shader
        vertex = loadAndCompileShader(vertexPath, "VERTEX", GL_VERTEX_SHADER);
        if (vertex == -1)
            return 0;
        if (!geometryPath.empty())
        {
            geometry = loadAndCompileShader(geometryPath, "GEOMETRY", GL_GEOMETRY_SHADER);
            if (geometry == -1)
                return 0;
        }
        fragment = loadAndCompileShader(fragmentPath, "FRAGMENT", GL_FRAGMENT_SHADER);
        if (fragment == -1)
            return 0;

        // shader Program
        glShaderProgram = glCreateProgram();
        glAttachShader(glShaderProgram, vertex);
        if (!geometryPath.empty())
            glAttachShader(glShaderProgram, geometry);
        glAttachShader(glShaderProgram, fragment);
        glLinkProgram(glShaderProgram);
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        if (!geometryPath.empty())
            glDeleteShader(geometry);
        glDeleteShader(fragment);
        return checkCompileErrors(glShaderProgram, "PROGRAM");
    }

    GLuint Shader::getglShaderProgram() const
    {
        return this->glShaderProgram;
    }

    void Shader::use() const
    {
        glUseProgram(glShaderProgram);
    }

    void Shader::setBool(const std::string &name, GLboolean value) const
    {
        glUniform1i(glGetUniformLocation(glShaderProgram, name.c_str()), (int)value);
    }

    void Shader::setInt(const std::string &name, GLint value) const
    {
        glUniform1i(glGetUniformLocation(glShaderProgram, name.c_str()), value);
    }

    void Shader::setFloat(const std::string &name, GLfloat value) const
    {
        glUniform1f(glGetUniformLocation(glShaderProgram, name.c_str()), value);
    }

    void Shader::setVec4f(const std::string &name, glm::vec4 value) const
    {
        glUniform4fv(glGetUniformLocation(glShaderProgram, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::setMatrix4fv(const std::string &name, glm::mat4 value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(glShaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    int Shader::checkCompileErrors(unsigned int shader, std::string type) const
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::stringstream err;
                err << "SHADER_COMPILATION_ERROR in " << type << " of type: " << infoLog;
                Log::e(err.str());
                return 0;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                Log::e("PROGRAM_LINKER_ERROR of type: ");
                Log::e(infoLog);
                return 0;
            }
        }
        return 1;
    }
} // namespace engine