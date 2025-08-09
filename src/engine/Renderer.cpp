#include "renderer.h"

#include <glad/glad.h>

Renderer::Renderer()
{
    glEnable(GL_DEPTH_TEST);
}

void Renderer::BeginFrame(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawMesh(const Mesh& mesh, const Shader& shader, const glm::mat4& mvp, const glm::vec3& light_pos, const glm::vec3& light_color)
{
    shader.use();
    shader.set_mat4("uMVP", mvp);
    shader.set_vec3("uLightPos", light_pos);
    shader.set_vec3("uLightColor", light_color);

    mesh.Bind();
    mesh.Draw();
}


