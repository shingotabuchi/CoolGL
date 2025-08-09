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

    glm::vec3 light_pos{0.0f, 0.0f, 10000.0f};
    glm::vec3 light_color{1.0f, 1.0f, 1.0f};

    void OnRender(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view) override;

private:
    Mesh mesh_{};
    Shader shader_{};
};


