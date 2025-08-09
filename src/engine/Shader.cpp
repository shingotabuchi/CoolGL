#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <vector>

static void throwIfShaderError(GLuint shader, const char* stage)
{
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());
        throw std::runtime_error(std::string("Shader compile error [") + stage + "]: " + log.data());
    }
}

static void throwIfLinkError(GLuint program)
{
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint logLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetProgramInfoLog(program, logLen, nullptr, log.data());
        throw std::runtime_error(std::string("Program link error: ") + log.data());
    }
}

Shader::Shader(const char* vertexSource, const char* fragmentSource)
{
    GLuint vs = compile(GL_VERTEX_SHADER, vertexSource);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentSource);
    link(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::Shader(Shader&& other) noexcept : programId_(other.programId_)
{
    other.programId_ = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        if (programId_)
        {
            glDeleteProgram(programId_);
        }
        programId_ = other.programId_;
        other.programId_ = 0;
    }
    return *this;
}

Shader::~Shader()
{
    if (programId_)
    {
        glDeleteProgram(programId_);
        programId_ = 0;
    }
}

GLuint Shader::compile(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    throwIfShaderError(shader, type == GL_VERTEX_SHADER ? "vertex" : "fragment");
    return shader;
}

void Shader::link(GLuint vertexShader, GLuint fragmentShader)
{
    programId_ = glCreateProgram();
    glAttachShader(programId_, vertexShader);
    glAttachShader(programId_, fragmentShader);
    glLinkProgram(programId_);
    throwIfLinkError(programId_);
}

void Shader::use() const
{
    glUseProgram(programId_);
}

void Shader::setMat4(const char* name, const glm::mat4& value) const
{
    GLint loc = glGetUniformLocation(programId_, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const char* name, const glm::vec3& value) const
{
    GLint loc = glGetUniformLocation(programId_, name);
    glUniform3fv(loc, 1, glm::value_ptr(value));
}


