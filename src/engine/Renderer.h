#pragma once

#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"

class Renderer
{
public:
    Renderer();

    void beginFrame(float r, float g, float b, float a);
    void drawMesh(const Mesh& mesh, const Shader& shader, const glm::mat4& mvp, const glm::vec3& lightPos, const glm::vec3& lightColor);
};


