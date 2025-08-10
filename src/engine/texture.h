#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

// Lightweight RAII wrapper around an OpenGL texture object name.
// Non-copyable, moveable. Automatically deletes the GL texture on destruction.
class Texture
{
public:
    Texture() noexcept = default;
    explicit Texture(GLuint id) noexcept : id_(id) {}

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept : id_(other.id_)
    {
        other.id_ = 0;
    }
    Texture& operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            release();
            id_ = other.id_;
            other.id_ = 0;
        }
        return *this;
    }

    ~Texture()
    {
        release();
    }

    // Returns the underlying GL texture name (0 if not set)
    GLuint id() const { return id_; }

    // True if holds a non-zero GL texture name
    bool is_valid() const { return id_ != 0; }

    // Replace the underlying GL texture name, deleting any previous one
    void reset(GLuint new_id)
    {
        if (id_ == new_id) return;
        release();
        id_ = new_id;
    }

    // Deletes the GL texture and sets id to 0
    void release()
    {
        if (id_ != 0)
        {
            glDeleteTextures(1, &id_);
            id_ = 0;
        }
    }

    // Convenience bind to a unit and target (defaults to GL_TEXTURE_2D at unit 0)
    void bind(GLenum target = GL_TEXTURE_2D, GLuint unit = 0) const
    {
        if (id_ == 0) return;
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(target, id_);
    }

    // Loads image from disk and computes its average color in linear [0..1].
    // Returns false if the image cannot be loaded.
    static bool ComputeAverageColorFromFile(const std::string& path,
                                            glm::vec3& out_average_rgb);

private:
    GLuint id_ = 0;
};


