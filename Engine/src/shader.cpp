#include "shader.hpp"

#include <string>
#include <fstream>
#include <sstream>

#include "log.hpp"
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

    std::string Shader::loadShaderSource(const std::string path) const
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

    GLuint Shader::compileShader(const std::string source, const std::string name, const GLenum shaderType) const
    {
        const char *shaderCode = source.c_str();
        GLuint result;
        result = glCreateShader(shaderType);
        glShaderSource(result, 1, &shaderCode, NULL);
        glCompileShader(result);
        if (!this->checkCompileErrors(result, name))
            return -1;
        else
            return result;
    }

    int Shader::loadShaderFromSource(const std::string vertexSource, const std::string geometrySource, const std::string fragmentSource)
    {
        GLuint vertex = 0;
        GLuint geometry = 0;
        GLuint fragment = 0;

        if (vertexSource.empty())
            return 0;
        vertex = compileShader(vertexSource, "VERTEX", GL_VERTEX_SHADER);
        if (vertex == -1)
            return 0;

        if (!geometrySource.empty())
            geometry = compileShader(geometrySource, "GEOMETRY", GL_GEOMETRY_SHADER);
            if (geometry == -1)
                    return 0;

        if (fragmentSource.empty())
            return 0;
        fragment = compileShader(fragmentSource, "FRAGMENT", GL_FRAGMENT_SHADER);
        if (fragment == -1)
            return 0;

        return compileProgram(vertex, geometry, fragment);
    }

    int Shader::loadShaderFromFile(const std::string vertexPath, const std::string geometryPath, const std::string fragmentPath)
    {
        // compile shader
        std::string vertexSource = loadShaderSource(vertexPath);

        std::string geometrySource = "";
        if (!geometryPath.empty())
            geometrySource = loadShaderSource(geometryPath);

        std::string fragmentSource = loadShaderSource(fragmentPath);
        
        return this->loadShaderFromSource(vertexSource, geometrySource, fragmentSource);
    }

    GLuint Shader::compileProgram(const GLuint vertex, const GLuint geometry, const GLuint fragment)
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