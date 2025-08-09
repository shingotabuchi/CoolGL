#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader() = default;
    Shader(const char* vertex_source, const char* fragment_source);

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    ~Shader();

    void use() const;
    GLuint id() const { return program_id_; }

    void set_mat4(const char* name, const glm::mat4& value) const;
    void set_vec3(const char* name, const glm::vec3& value) const;

private:
    GLuint compile(GLenum type, const char* source);
    void link(GLuint vertex_shader, GLuint fragment_shader);

private:
    GLuint program_id_ = 0;
};


