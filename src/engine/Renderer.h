#pragma once

#include <glm/glm.hpp>
#include "mesh.h"
#include "shader.h"

class Renderer
{
public:
    Renderer();

    void BeginFrame(float r, float g, float b, float a);
    void DrawMesh(const Mesh& mesh, const Shader& shader, const glm::mat4& mvp, const glm::vec3& light_pos, const glm::vec3& light_color);
};


