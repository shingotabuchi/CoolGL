#pragma once

#include "component.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include <glm/glm.hpp>
#include <memory>

class Renderer;

class MeshRenderer : public Component
{
public:
    MeshRenderer() = default;
    // Construct with owned GPU resources; stored as shared so clones can share them
    MeshRenderer(Mesh mesh, Shader shader)
        : mesh_(std::make_shared<Mesh>(std::move(mesh)))
        , shader_(std::make_shared<Shader>(std::move(shader))) {}

    // Construct with shared resources directly
    MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader)
        : mesh_(std::move(mesh)), shader_(std::move(shader)) {}

    // Deprecated: lighting now comes from Scene's active Light
    glm::vec3 light_color{1.0f, 1.0f, 1.0f};

    void OnRender(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view) override;
    void OnAttach() override { cached_transform_ = nullptr; }

    // Optional texture used as diffuse/albedo. Shared so clones can share the GPU resource.
    std::shared_ptr<Texture> diffuse_texture;

    // Optional per-material ambient multiplier (defaults to 1). The final
    // ambient used is scene.ambient_color * material_ambient_multiplier.
    glm::vec3 material_ambient_multiplier{1.0f, 1.0f, 1.0f};

    std::unique_ptr<Component> Clone() const override
    {
        auto copy = std::make_unique<MeshRenderer>(mesh_, shader_);
        copy->diffuse_texture = diffuse_texture;
        copy->material_ambient_multiplier = material_ambient_multiplier;
        copy->light_color = light_color;
        return copy;
    }

private:
    std::shared_ptr<Mesh> mesh_{};
    std::shared_ptr<Shader> shader_{};
    // Cache to avoid per-frame GetComponent lookup
    mutable class Transform* cached_transform_ = nullptr;
};


