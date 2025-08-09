#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader() = default;
    Shader(const char* vertexSource, const char* fragmentSource);

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    ~Shader();

    void use() const;
    GLuint id() const { return programId_; }

    // Uniform helpers
    void setMat4(const char* name, const glm::mat4& value) const;
    void setVec3(const char* name, const glm::vec3& value) const;

private:
    GLuint compile(GLenum type, const char* source);
    void link(GLuint vertexShader, GLuint fragmentShader);

private:
    GLuint programId_ = 0;
};


