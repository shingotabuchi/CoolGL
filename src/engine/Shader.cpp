#include "shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <vector>

static void ThrowIfShaderError(GLuint shader, const char* stage)
{
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint log_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
        std::vector<char> log(log_len);
        glGetShaderInfoLog(shader, log_len, nullptr, log.data());
        throw std::runtime_error(std::string("Shader compile error [") + stage + "]: " + log.data());
    }
}

static void ThrowIfLinkError(GLuint program)
{
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint log_len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
        std::vector<char> log(log_len);
        glGetProgramInfoLog(program, log_len, nullptr, log.data());
        throw std::runtime_error(std::string("Program link error: ") + log.data());
    }
}

Shader::Shader(const char* vertex_source, const char* fragment_source)
{
    GLuint vs = compile(GL_VERTEX_SHADER, vertex_source);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragment_source);
    link(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::Shader(Shader&& other) noexcept : program_id_(other.program_id_)
{
    other.program_id_ = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        if (program_id_)
        {
            glDeleteProgram(program_id_);
        }
        program_id_ = other.program_id_;
        other.program_id_ = 0;
    }
    return *this;
}

Shader::~Shader()
{
    if (program_id_)
    {
        glDeleteProgram(program_id_);
        program_id_ = 0;
    }
}

GLuint Shader::compile(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    ThrowIfShaderError(shader, type == GL_VERTEX_SHADER ? "vertex" : "fragment");
    return shader;
}

void Shader::link(GLuint vertex_shader, GLuint fragment_shader)
{
    program_id_ = glCreateProgram();
    glAttachShader(program_id_, vertex_shader);
    glAttachShader(program_id_, fragment_shader);
    glLinkProgram(program_id_);
    ThrowIfLinkError(program_id_);
}

void Shader::use() const
{
    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    if (static_cast<GLuint>(current) != program_id_)
    {
        glUseProgram(program_id_);
    }
}

void Shader::set_mat4(const char* name, const glm::mat4& value) const
{
    GLint loc = get_uniform_location_cached(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_vec3(const char* name, const glm::vec3& value) const
{
    GLint loc = get_uniform_location_cached(name);
    glUniform3fv(loc, 1, glm::value_ptr(value));
}

GLint Shader::get_uniform_location_cached(const char* name) const
{
    auto it = uniform_location_cache_.find(name);
    if (it != uniform_location_cache_.end())
    {
        return it->second;
    }
    GLint loc = glGetUniformLocation(program_id_, name);
    uniform_location_cache_.emplace(name, loc);
    return loc;
}

void Shader::set_mat4(GLint location, const glm::mat4& value) const
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_vec3(GLint location, const glm::vec3& value) const
{
    glUniform3fv(location, 1, glm::value_ptr(value));
}


