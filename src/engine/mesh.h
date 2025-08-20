#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

struct MeshVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

class Mesh
{
public:
    int instance_id;
    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;
    Mesh() = default;
    Mesh(const std::vector<MeshVertex> &vertices, const std::vector<unsigned int> &indices);

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;
    Mesh(Mesh &&other) noexcept;
    Mesh &operator=(Mesh &&other) noexcept;
    ~Mesh();

    void CreateInstanceBuffer(const std::vector<glm::mat4> &model_matrices);

    void Bind() const;
    void Draw() const;
    void DrawInstanced() const;

private:
    void CreateBuffers(const std::vector<MeshVertex> &vertices, const std::vector<unsigned int> &indices);

private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;
    GLuint instance_vbo_ = 0;
    GLsizei index_count_ = 0;
    int instance_size_;
    // Future: consider primitive restart or 32-bit indices based on size
};
