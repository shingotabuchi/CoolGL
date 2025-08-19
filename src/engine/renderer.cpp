#include "renderer.h"
#include <glad/glad.h>
#include <iostream>

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

// void Renderer::UpdateLightState(int light_count, const glm::vec3 *light_dirs_object_space, const glm::vec3 *light_colors)
// {
// }

void Renderer::DrawMesh(const Mesh &mesh,
                        const Shader &shader,
                        CachedLightState &light_state,
                        const glm::mat4 &mvp,
                        int light_count,
                        const glm::vec3 *light_dirs_object_space,
                        const glm::vec3 *light_colors)
{
    shader.use();
    const GLint locMVP = shader.get_uniform_location_cached("uMVP");
    const GLint locLightCount = shader.get_uniform_location_cached("uLightCount");
    const GLint locLightDirs0 = shader.get_uniform_location_cached("uLightDirs[0]");
    const GLint locLightColors0 = shader.get_uniform_location_cached("uLightColors[0]");
    shader.set_mat4(locMVP, mvp);

    // Avoid redundant uniform updates for lights when possible
    const GLuint currentProgram = shader.id();
    bool programChanged = (light_state.program != currentProgram);
    bool countChanged = (light_state.count != light_count) || programChanged;
    bool dirsChanged = programChanged || countChanged;
    bool colorsChanged = programChanged || countChanged;

    if (!programChanged && !countChanged)
    {
        // Compare arrays only up to light_count
        for (int i = 0; i < light_count && !dirsChanged; ++i)
        {
            const glm::vec3 diff = light_state.dirs[i] - light_dirs_object_space[i];
            if (glm::dot(diff, diff) > 1e-12f)
            {
                dirsChanged = true;
            }
        }
        for (int i = 0; i < light_count && !colorsChanged; ++i)
        {
            const glm::vec3 diff = light_state.colors[i] - light_colors[i];
            if (glm::dot(diff, diff) > 1e-12f)
            {
                colorsChanged = true;
            }
        }
    }

    if (countChanged)
    {
        shader.set_int(locLightCount, light_count);
        light_state.count = light_count;
    }
    if (dirsChanged)
    {
        shader.set_vec3_array(locLightDirs0, light_dirs_object_space, light_count);
        for (int i = 0; i < light_count; ++i)
            light_state.dirs[i] = light_dirs_object_space[i];
    }
    if (colorsChanged)
    {
        shader.set_vec3_array(locLightColors0, light_colors, light_count);
        for (int i = 0; i < light_count; ++i)
            light_state.colors[i] = light_colors[i];
    }
    light_state.program = currentProgram;

    mesh.Bind();
    mesh.Draw();
}

// void Renderer::InitializeShadowMap()
// {
//     // Create the depth texture
//     glGenTextures(1, &m_shadowMapTexture);
//     glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//     float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Areas outside map are not in shadow
//     glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

//     // Create the FBO
//     glGenFramebuffers(1, &m_shadowMapFBO);
//     glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMapTexture, 0);
//     glDrawBuffer(GL_NONE);
//     glReadBuffer(GL_NONE);

//     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//     {
//         // Handle error
//     }
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);

//     // Create the depth shader
//     m_depthShader = std::make_unique<Shader>(kDepthVS, kDepthFS);
// }
