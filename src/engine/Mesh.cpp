#include "mesh.h"

Mesh::Mesh(const std::vector<MeshVertex>& vertices, const std::vector<unsigned int>& indices)
{
    CreateBuffers(vertices, indices);
}

Mesh::Mesh(Mesh&& other) noexcept
    : vao_(other.vao_), vbo_(other.vbo_), ebo_(other.ebo_), index_count_(other.index_count_)
{
    other.vao_ = other.vbo_ = other.ebo_ = 0;
    other.index_count_ = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        if (vao_) glDeleteVertexArrays(1, &vao_);
        if (vbo_) glDeleteBuffers(1, &vbo_);
        if (ebo_) glDeleteBuffers(1, &ebo_);
        vao_ = other.vao_;
        vbo_ = other.vbo_;
        ebo_ = other.ebo_;
        index_count_ = other.index_count_;
        other.vao_ = other.vbo_ = other.ebo_ = 0;
        other.index_count_ = 0;
    }
    return *this;
}

Mesh::~Mesh()
{
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (ebo_) glDeleteBuffers(1, &ebo_);
}

void Mesh::CreateBuffers(const std::vector<MeshVertex>& vertices, const std::vector<unsigned int>& indices)
{
    index_count_ = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::Bind() const
{
    // Cache last bound VAO per thread to avoid redundant driver calls
    static thread_local GLuint last_vao = 0;
    if (last_vao != vao_)
    {
        glBindVertexArray(vao_);
        last_vao = vao_;
    }
}

void Mesh::Draw() const
{
    glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, 0);
}


