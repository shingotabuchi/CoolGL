#pragma once

#include "component.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "material.h"
#include "renderer.h"
#include <glm/glm.hpp>
#include <memory>

class Transform;

class MeshRenderer : public Component
{
public:
    MeshRenderer() = default;
    enum class RenderMode
    {
        Lit,
        Unlit,
        Skybox
    };
    // Construct with owned GPU resources; stored as shared so clones can share them
    MeshRenderer(Mesh mesh, Shader shader)
        : mesh_(std::make_shared<Mesh>(std::move(mesh))), shader_(std::make_shared<Shader>(std::move(shader))) {}

    // Construct with shared resources directly
    MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader)
        : mesh_(std::move(mesh)), shader_(std::move(shader)) {}

    // Construct with a Material. Shader and textures are sourced from the material.
    MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
        : mesh_(std::move(mesh)), material_(std::move(material)) {}

    // Deprecated: lighting now comes from Scene's active Light
    glm::vec3 light_color{1.0f, 1.0f, 1.0f};

    void OnRender(Renderer &renderer, const glm::mat4 &projection, const glm::mat4 &view) override;
    void OnAttach() override { cached_transform_ = nullptr; }

    // Optional texture used as diffuse/albedo (legacy path, prefer Material).
    std::shared_ptr<Texture> diffuse_texture;

    // Optional per-material ambient multiplier (defaults to 1). The final
    // ambient used is scene.ambient_color * material_ambient_multiplier.
    glm::vec3 material_ambient_multiplier{1.0f, 1.0f, 1.0f};

    // Simple material controls for the lit shader
    glm::vec3 color{1.0f, 1.0f, 1.0f};
    float smoothness{0.5f};

    std::unique_ptr<Component> Clone() const override
    {
        auto copy = std::make_unique<MeshRenderer>(mesh_, shader_);
        copy->diffuse_texture = diffuse_texture;
        copy->material_ = material_;
        copy->material_ambient_multiplier = material_ambient_multiplier;
        copy->color = color;
        copy->smoothness = smoothness;
        copy->light_color = light_color;
        copy->render_mode = render_mode;
        return copy;
    }

    // Rendering mode selector (defaults to Lit)
    RenderMode render_mode = RenderMode::Lit;

    // Helpers to set resources after default construction
    void SetMesh(std::shared_ptr<Mesh> mesh) { mesh_ = std::move(mesh); }
    void SetShader(std::shared_ptr<Shader> shader) { shader_ = std::move(shader); }
    void SetMaterial(std::shared_ptr<Material> material) { material_ = std::move(material); }
    std::shared_ptr<Mesh> GetMesh() const { return mesh_; }
    std::shared_ptr<Shader> GetShader() const { return shader_; }
    std::shared_ptr<Material> GetMaterial() const { return material_; }

    // Utility: create or obtain a shared unit cube mesh (internally uses MeshCreator)
    static std::shared_ptr<Mesh> CreateUnitCube();

private:
    std::shared_ptr<Mesh> mesh_{};
    std::shared_ptr<Shader> shader_{};
    std::shared_ptr<Material> material_{};
    Transform *cached_transform_ = nullptr;
    Renderer::CachedLightState cached_light_state_{};
};
