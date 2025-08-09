#include "Renderer.h"

#include <glad/glad.h>

Renderer::Renderer()
{
    glEnable(GL_DEPTH_TEST);
}

void Renderer::beginFrame(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::drawMesh(const Mesh& mesh, const Shader& shader, const glm::mat4& mvp, const glm::vec3& lightPos, const glm::vec3& lightColor)
{
    shader.use();
    shader.setMat4("uMVP", mvp);
    shader.setVec3("uLightPos", lightPos);
    shader.setVec3("uLightColor", lightColor);

    mesh.bind();
    mesh.draw();
}


