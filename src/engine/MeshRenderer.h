#pragma once

#include "Component.h"
#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>

class MeshRenderer : public Component
{
public:
    MeshRenderer() = default;
    MeshRenderer(Mesh mesh, Shader shader) : mesh_(std::move(mesh)), shader_(std::move(shader)) {}

    glm::vec3 lightPos{0.0f, 0.0f, 10000.0f};
    glm::vec3 lightColor{1.0f, 1.0f, 1.0f};

    void onRender(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view) override;

private:
    Mesh mesh_{};
    Shader shader_{};
};


