#include "core/shader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <glm/ext.hpp>

#include "core/log.hpp"

namespace engine
{
    Shader::Shader()
    {
        glShaderProgram = 0;
    }

    void Shader::unloadResource()
    {
        if (glShaderProgram != 0)
        {
            glDeleteProgram(glShaderProgram);
            glShaderProgram = 0;
        }
    }

    bool Shader::loadResource(ResourceManager &resourceManager, va_list args)
    {
        std::string vertexPath(va_arg(args, const char *));
        std::string geometryPath(va_arg(args, const char *));
        std::string fragmentPath(va_arg(args, const char *));

        std::string vertexSource = loadShaderSource(vertexPath);
        if (vertexSource.empty())
            return -1;

        std::string geometrySource = "";
        if (!geometryPath.empty())
            geometrySource = loadShaderSource(geometryPath);

        std::string fragmentSource = loadShaderSource(fragmentPath);
        if (fragmentSource.empty())
            return -1;

        if (!compile(vertexSource, geometrySource, fragmentSource))
            return 0;
        return -1;
    }

    std::string Shader::loadShaderSource(const std::string &path) const
    {
        std::string source;
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            file.open(path);
            std::stringstream sourceStream;
            sourceStream << file.rdbuf();
            file.close();
            source = sourceStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::stringstream error;
            error << "SHADER::FILE_NOT_SUCCESFULLY_READ. FILE: " << path;
            Log::e(error.str());
            return "";
        }
        return source;
    }

    GLuint Shader::compileShader(const std::string &source, const std::string &name, const GLenum shaderType) const
    {
        const char *shaderCode = source.c_str();
        GLuint result;
        result = glCreateShader(shaderType);
        glShaderSource(result, 1, &shaderCode, NULL);
        glCompileShader(result);

        bool errors = checkCompileErrors(result, name);
        if (errors)
            return 0;
        else
            return result;
    }

    bool Shader::compileProgram(GLuint vertex, GLuint geometry, GLuint fragment)
    {
        // shader Program
        glShaderProgram = glCreateProgram();
        glAttachShader(glShaderProgram, vertex);
        if (geometry != 0)
            glAttachShader(glShaderProgram, geometry);
        glAttachShader(glShaderProgram, fragment);
        glLinkProgram(glShaderProgram);

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        if (geometry != 0)
            glDeleteShader(geometry);
        glDeleteShader(fragment);

        bool errors = checkCompileErrors(glShaderProgram, "PROGRAM");
        if (errors)
        {
            unloadResource();
            return false;
        }

        return true;
    }

    bool Shader::compile(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource)
    {
        GLuint vertex = 0;
        GLuint geometry = 0;
        GLuint fragment = 0;

        if (vertexSource.empty())
            return false;
        vertex = compileShader(vertexSource, "VERTEX", GL_VERTEX_SHADER);
        if (!vertex)
            return false;

        if (!geometrySource.empty())
        {
            geometry = compileShader(geometrySource, "GEOMETRY", GL_GEOMETRY_SHADER);
            if (!geometry)
                return false;
        }

        if (fragmentSource.empty())
            return false;
        fragment = compileShader(fragmentSource, "FRAGMENT", GL_FRAGMENT_SHADER);
        if (!fragment)
            return false;

        bool compileSuccess = compileProgram(vertex, geometry, fragment);
        return compileSuccess;
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

    bool Shader::checkCompileErrors(GLuint shader, std::string type) const
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
                return true;
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
                return true;
            }
        }
        return false;
    }
} // namespace engine