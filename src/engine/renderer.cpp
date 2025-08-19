#include "renderer.h"
#include <glad/glad.h>
#include <iostream>

Renderer::CachedLightState Renderer::s_cached_light_state_{};

// A simple shader for rendering depth only
static const char *kDepthVS = R"glsl(
    #version 410 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 uMVP; // LightSpaceMatrix * ModelMatrix
    void main() {
        gl_Position = uMVP * vec4(aPos, 1.0);
    }
)glsl";

static const char *kDepthFS = R"glsl(
    #version 410 core
    void main() {
        // No color output needed
    }
)glsl";

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

void Renderer::UpdateLightState(int light_count, const glm::vec3 *light_dirs, const glm::vec3 *light_colors, const glm::vec3 cam_pos)
{
    s_cached_light_state_.has_changed = s_cached_light_state_.count != light_count;

    for (int i = 0; i < light_count && !s_cached_light_state_.has_changed; ++i)
    {
        const glm::vec3 diff = s_cached_light_state_.dirs[i] - light_dirs[i];
        if (glm::dot(diff, diff) > 1e-12f)
        {
            s_cached_light_state_.has_changed = true;
        }
    }
    for (int i = 0; i < light_count && !s_cached_light_state_.has_changed; ++i)
    {
        const glm::vec3 diff = s_cached_light_state_.colors[i] - light_colors[i];
        if (glm::dot(diff, diff) > 1e-12f)
        {
            s_cached_light_state_.has_changed = true;
        }
    }
    if (!s_cached_light_state_.has_changed)
    {
        const glm::vec3 diff = s_cached_light_state_.cam_pos - cam_pos;
        if (glm::dot(diff, diff) > 1e-12f)
        {
            s_cached_light_state_.has_changed = true;
        }
    }

    if (s_cached_light_state_.has_changed)
    {
        s_cached_light_state_.count = light_count;
        for (int i = 0; i < light_count; ++i)
            s_cached_light_state_.dirs[i] = light_dirs[i];
        for (int i = 0; i < light_count; ++i)
            s_cached_light_state_.colors[i] = light_colors[i];
        s_cached_light_state_.cam_pos = cam_pos;
    }
}

void Renderer::DrawMesh(const Mesh &mesh,
                        const Shader &shader,
                        const glm::mat4 &mvp)
{
    shader.use();
    const GLint locMVP = shader.get_uniform_location_cached("uMVP");
    const GLint locLightCount = shader.get_uniform_location_cached("uLightCount");
    const GLint locLightDirs0 = shader.get_uniform_location_cached("uLightDirs[0]");
    const GLint locLightColors0 = shader.get_uniform_location_cached("uLightColors[0]");
    shader.set_mat4(locMVP, mvp);

    if (s_cached_light_state_.has_changed)
    {
        auto count = s_cached_light_state_.count;
        shader.set_int(locLightCount, count);
        shader.set_vec3_array(locLightDirs0, s_cached_light_state_.dirs, count);
        shader.set_vec3_array(locLightColors0, s_cached_light_state_.colors, count);
        shader.set_vec3("uCamPos", s_cached_light_state_.cam_pos);
    }

    mesh.Bind();
    mesh.Draw();
}