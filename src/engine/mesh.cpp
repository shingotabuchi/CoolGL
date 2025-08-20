#include "mesh.h"

Mesh::Mesh(const std::vector<MeshVertex> &vertices, const std::vector<unsigned int> &indices)
{
    CreateBuffers(vertices, indices);
}

Mesh::Mesh(Mesh &&other) noexcept
    : vao_(other.vao_), vbo_(other.vbo_), ebo_(other.ebo_),
      instance_vbo_(other.instance_vbo_), index_count_(other.index_count_)
{
    other.vao_ = other.vbo_ = other.ebo_ = other.instance_vbo_ = 0;
    other.index_count_ = 0;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept
{
    if (this != &other)
    {
        if (vao_)
            glDeleteVertexArrays(1, &vao_);
        if (vbo_)
            glDeleteBuffers(1, &vbo_);
        if (ebo_)
            glDeleteBuffers(1, &ebo_);
        if (instance_vbo_)
            glDeleteBuffers(1, &instance_vbo_);

        vao_ = other.vao_;
        vbo_ = other.vbo_;
        ebo_ = other.ebo_;
        instance_vbo_ = other.instance_vbo_;
        index_count_ = other.index_count_;
        other.vao_ = other.vbo_ = other.ebo_ = other.instance_vbo_ = 0;
        other.index_count_ = 0;
    }
    return *this;
}

Mesh::~Mesh()
{
    if (vao_)
        glDeleteVertexArrays(1, &vao_);
    if (vbo_)
        glDeleteBuffers(1, &vbo_);
    if (ebo_)
        glDeleteBuffers(1, &ebo_);
    if (instance_vbo_)
        glDeleteBuffers(1, &instance_vbo_);
}

void Mesh::CreateBuffers(const std::vector<MeshVertex> &vertices, const std::vector<unsigned int> &indices)
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::Bind() const
{
    glBindVertexArray(vao_);
}

void Mesh::Draw() const
{
    glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, 0);
}

void Mesh::CreateInstanceBuffer(const std::vector<glm::mat4> &model_matrices)
{
    instance_size_ = model_matrices.size();
    // If we already have an instance buffer, delete it first
    if (instance_vbo_)
    {
        glDeleteBuffers(1, &instance_vbo_);
    }

    glGenBuffers(1, &instance_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, instance_vbo_);
    glBufferData(GL_ARRAY_BUFFER, instance_size_ * sizeof(glm::mat4), model_matrices.data(), GL_STATIC_DRAW);

    // We need to tell the VAO how to interpret this new buffer data.
    glBindVertexArray(vao_);

    // A mat4 is equivalent to 4 vec4s. We need to set vertex attributes for each of them.
    // Your existing attributes are at locations 0, 1, 2. We'll start at 3.
    const GLuint starting_attrib_location = 3;
    for (int i = 0; i < 4; ++i)
    {
        GLuint current_location = starting_attrib_location + i;
        glEnableVertexAttribArray(current_location);
        glVertexAttribPointer(
            current_location,               // Attribute location
            4,                              // Number of components (a vec4)
            GL_FLOAT,                       // Type
            GL_FALSE,                       // Normalize
            sizeof(glm::mat4),              // Stride: total size of one instance's data
            (void *)(i * sizeof(glm::vec4)) // Offset to this column
        );
        // This is the key! It tells OpenGL to advance this attribute only once per instance.
        glVertexAttribDivisor(current_location, 1);
    }

    glBindVertexArray(0);
}

// ✨ Here is the new instanced draw call implementation ✨
void Mesh::DrawInstanced() const
{
    // The second-to-last argument is the number of instances to render.
    glDrawElementsInstanced(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, 0, instance_size_);
}