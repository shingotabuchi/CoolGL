#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>

class Shader;
class Texture;

// Simple material holding visual properties and resources for a mesh.
// The material owns references to a compiled Shader and a loaded albedo Texture,
// and exposes authoring properties like shader/texture paths, color, and smoothness.
class Material
{
public:
    Material() = default;

    // Authoring properties
    std::string vertex_shader_path{};
    std::string fragment_shader_path{};
    std::string albedo_texture_path{};

    glm::vec3 color{1.0f, 1.0f, 1.0f};
    float smoothness{0.5f};

    // Lazy-load/compile resources if needed. Returns true on success.
    bool EnsureResourcesLoaded();

    // Accessors to loaded resources
    std::shared_ptr<Shader> GetShader() const { return shader_; }
    std::shared_ptr<Texture> GetAlbedoTexture() const { return albedo_texture_; }

private:
    std::shared_ptr<Shader> shader_{};
    std::shared_ptr<Texture> albedo_texture_{};
};


