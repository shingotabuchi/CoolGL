#include "renderer.h"

#include <glad/glad.h>

Renderer::Renderer()
{
    glEnable(GL_DEPTH_TEST);
    // Enable back-face culling for fewer fragment shader invocations
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Renderer::BeginFrame(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawMesh(const Mesh& mesh, const Shader& shader, const glm::mat4& mvp, const glm::vec3& light_pos, const glm::vec3& light_color)
{
    shader.use();
    const GLint locMVP = shader.get_uniform_location_cached("uMVP");
    const GLint locLightPos = shader.get_uniform_location_cached("uLightPos");
    const GLint locLightColor = shader.get_uniform_location_cached("uLightColor");
    shader.set_mat4(locMVP, mvp);
    shader.set_vec3(locLightPos, light_pos);
    shader.set_vec3(locLightColor, light_color);

    mesh.Bind();
    mesh.Draw();
}


