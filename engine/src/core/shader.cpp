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
        _gl_program = 0;
    }

    void Shader::unload_resource()
    {
        if (_gl_program != 0)
        {
            glDeleteProgram(_gl_program);
            _gl_program = 0;
        }
    }

    bool Shader::load_resource(ResourceManager &_resource_manager, va_list args)
    {
        std::string vertexPath(va_arg(args, const char *));
        std::string geometryPath(va_arg(args, const char *));
        std::string fragmentPath(va_arg(args, const char *));

        std::string vertexSource = load_shader_source(vertexPath);
        if (vertexSource.empty())
            return -1;

        std::string geometrySource = "";
        if (!geometryPath.empty())
            geometrySource = load_shader_source(geometryPath);

        std::string fragmentSource = load_shader_source(fragmentPath);
        if (fragmentSource.empty())
            return -1;

        if (!compile(vertexSource, geometrySource, fragmentSource))
            return 0;
        return -1;
    }

    std::string Shader::load_shader_source(const std::string &path) const
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

    GLuint Shader::compile_shader(const std::string &source, const std::string &name, const GLenum shaderType) const
    {
        const char *shaderCode = source.c_str();
        GLuint result;
        result = glCreateShader(shaderType);
        glShaderSource(result, 1, &shaderCode, NULL);
        glCompileShader(result);

        bool errors = check_compile_errors(result, name);
        if (errors)
            return 0;
        else
            return result;
    }

    bool Shader::compile_program(GLuint vertex, GLuint geometry, GLuint fragment)
    {
        // shader Program
        _gl_program = glCreateProgram();
        glAttachShader(_gl_program, vertex);
        if (geometry != 0)
            glAttachShader(_gl_program, geometry);
        glAttachShader(_gl_program, fragment);
        glLinkProgram(_gl_program);

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        if (geometry != 0)
            glDeleteShader(geometry);
        glDeleteShader(fragment);

        bool errors = check_compile_errors(_gl_program, "PROGRAM");
        if (errors)
        {
            unload_resource();
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
        vertex = compile_shader(vertexSource, "VERTEX", GL_VERTEX_SHADER);
        if (!vertex)
            return false;

        if (!geometrySource.empty())
        {
            geometry = compile_shader(geometrySource, "GEOMETRY", GL_GEOMETRY_SHADER);
            if (!geometry)
                return false;
        }

        if (fragmentSource.empty())
            return false;
        fragment = compile_shader(fragmentSource, "FRAGMENT", GL_FRAGMENT_SHADER);
        if (!fragment)
            return false;

        bool compileSuccess = compile_program(vertex, geometry, fragment);
        return compileSuccess;
    }

    void Shader::use() const
    {
        glUseProgram(_gl_program);
    }

    void Shader::set(const std::string &name, GLboolean value) const
    {
        glUniform1i(glGetUniformLocation(_gl_program, name.c_str()), (int)value);
    }

    void Shader::set(const std::string &name, GLint value) const
    {
        glUniform1i(glGetUniformLocation(_gl_program, name.c_str()), value);
    }

    void Shader::set(const std::string &name, GLfloat value) const
    {
        glUniform1f(glGetUniformLocation(_gl_program, name.c_str()), value);
    }

    void Shader::set(const std::string &name, glm::vec4 value) const
    {
        glUniform4fv(glGetUniformLocation(_gl_program, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::set(const std::string &name, glm::mat4 value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(_gl_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    bool Shader::check_compile_errors(GLuint shader, std::string type) const
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