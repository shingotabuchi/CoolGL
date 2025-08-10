#pragma once

#include "component.h"
#include "mesh.h"
#include "shader.h"
#include <glm/glm.hpp>

class Renderer;

class MeshRenderer : public Component
{
public:
    MeshRenderer() = default;
    MeshRenderer(Mesh mesh, Shader shader) : mesh_(std::move(mesh)), shader_(std::move(shader)) {}

    // Deprecated: lighting now comes from Scene's active Light
    glm::vec3 light_color{1.0f, 1.0f, 1.0f};

    void OnRender(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view) override;
    void OnAttach() override { cached_transform_ = nullptr; }

    // Optional GL texture ID to bind as diffuse texture to unit 0
    unsigned int diffuse_texture = 0;

private:
    Mesh mesh_{};
    Shader shader_{};
    // Cache to avoid per-frame GetComponent lookup
    mutable class Transform* cached_transform_ = nullptr;
};


