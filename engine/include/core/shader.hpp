#pragma once

#include <string>

#include "glad/glad.h"
#include <glm/glm.hpp>

#include "resource.hpp"

namespace engine
{
    class Shader : public Resource
    {
    private:
        GLuint _gl_program = 0;

        // Returns true if there were errors
        bool check_compile_errors(GLuint shader, std::string type) const;
        GLuint compile_shader(const std::string &source, const std::string &name, GLenum shader_type) const;
        // Returns true if compilation was successful
        bool compile_program(GLuint vertex, GLuint geometry, GLuint fragment);

        std::string load_shader_source(const std::string &path) const;

    public:
        Shader();

        virtual bool load_resource(ResourceManager &_resource_manager, va_list args) override;
        virtual void unload_resource() override;

        void set_bool(const std::string &name, GLboolean value) const;
        void set_int(const std::string &name, GLint value) const;
        void set_float(const std::string &name, GLfloat value) const;
        void set_vec4(const std::string &name, glm::vec4 value) const;
        void set_mat4(const std::string &name, glm::mat4 value) const;

        bool compile(const std::string &vertex_source, const std::string &geometry_source, const std::string &fragment_source);
        void use() const;

        /*
            @return a GLuint object that represents the shader program
        */
        GLuint gl_program() const { return _gl_program; }
    };
} // namespace engine
