#pragma once

#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader() = default;
    Shader(const char *vertex_source, const char *fragment_source);
    static Shader FromFiles(const std::string &vertex_path, const std::string &fragment_path);

    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    Shader(Shader &&other) noexcept;
    Shader &operator=(Shader &&other) noexcept;

    ~Shader();

    void use() const;
    GLuint id() const { return program_id_; }

    void set_mat4(const char *name, const glm::mat4 &value) const;
    void set_vec3(const char *name, const glm::vec3 &value) const;
    void set_float(const char *name, float value) const;
    void set_int(const char *name, int value) const;

    // Fast setters using cached uniform locations (call once you know the name)
    GLint get_uniform_location_cached(const char *name) const;
    void set_mat4(GLint location, const glm::mat4 &value) const;
    void set_vec3(GLint location, const glm::vec3 &value) const;
    void set_vec3_array(GLint location, const glm::vec3 *values, int count) const;
    void set_float(GLint location, float value) const;
    void set_int(GLint location, int value) const;

private:
    GLuint compile(GLenum type, const char *source);
    void link(GLuint vertex_shader, GLuint fragment_shader);

private:
    GLuint program_id_ = 0;
    // Cache uniform name -> location
    mutable std::unordered_map<std::string, GLint> uniform_location_cache_{};
};
