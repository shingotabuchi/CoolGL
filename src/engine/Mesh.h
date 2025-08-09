#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

struct MeshVertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

class Mesh
{
public:
    Mesh() = default;
    Mesh(const std::vector<MeshVertex>& vertices, const std::vector<unsigned int>& indices);

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    ~Mesh();

    void Bind() const;
    void Draw() const;

private:
    void CreateBuffers(const std::vector<MeshVertex>& vertices, const std::vector<unsigned int>& indices);

private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;
    GLsizei index_count_ = 0;
};


