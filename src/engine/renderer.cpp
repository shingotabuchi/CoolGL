#include "renderer.h"
#include <glad/glad.h>
#include <iostream>

Renderer::CachedLightState Renderer::s_cached_light_state_{};

// Enhanced depth shader with minimal bias
static const char *kDepthVS = R"glsl(
    #version 410 core
    layout (location = 0) in vec3 aPos;
    layout (location = 3) in mat4 aInstanceMatrix;
    
    uniform mat4 uMVP; // LightSpaceMatrix * ModelMatrix
    uniform mat4 uModel;
    uniform bool u_isInstanced;
    
    void main() {
        mat4 finalModelMatrix = u_isInstanced ? aInstanceMatrix : uModel;
        gl_Position = uMVP * vec4(aPos, 1.0);
    }
)glsl";

static const char *kDepthFS = R"glsl(
    #version 410 core
    
    void main() {
        // Use very minimal bias in depth pass
        // Slightly reduce depth to ensure self-shadowing works properly
        gl_FragDepth = gl_FragCoord.z - 0.0001;
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
                        const Shader &shader)
{
    shader.use();
    const GLint locLightCount = shader.get_uniform_location_cached("uLightCount");
    const GLint locLightDirs0 = shader.get_uniform_location_cached("uLightDirs[0]");
    const GLint locLightColors0 = shader.get_uniform_location_cached("uLightColors[0]");

    if (s_cached_light_state_.has_changed)
    {
        auto count = s_cached_light_state_.count;
        shader.set_int(locLightCount, count);
        shader.set_vec3_array(locLightDirs0, s_cached_light_state_.dirs, count);
        shader.set_vec3_array(locLightColors0, s_cached_light_state_.colors, count);
        shader.set_vec3("uCamPos", s_cached_light_state_.cam_pos);
    }

    const bool is_instanced = mesh.instance_id > 0;

    shader.set_int("u_isInstanced", is_instanced);
    mesh.Bind();

    if (is_instanced)
    {
        mesh.DrawInstanced();
    }
    else
    {
        mesh.Draw();
    }
}

void Renderer::InitializeShadowMap(int width, int height)
{
    m_shadowMapWidth = width;
    m_shadowMapHeight = height;

    // Create the depth texture
    glGenTextures(1, &m_shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    // Enable hardware PCF (Percentage Closer Filtering) for smooth shadows
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Enable depth comparison for shadow testing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Areas outside map are not in shadow
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Create the FBO
    glGenFramebuffers(1, &m_shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        // Handle error
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create the depth shader
    m_depthShader = std::make_unique<Shader>(kDepthVS, kDepthFS);
}

void Renderer::BeginShadowPass(const glm::mat4 &lightSpaceMatrix, const glm::vec3 &lightDir)
{
    m_lightSpaceMatrix = lightSpaceMatrix; // Cache it for drawing
    m_currentLightDir = lightDir;          // Cache light direction
    glViewport(0, 0, m_shadowMapWidth, m_shadowMapHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    m_depthShader->use();
    glCullFace(GL_FRONT); // Fix for peter-panning
}

void Renderer::SetViewport(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Renderer::EndShadowPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK); // Restore back-face culling
    // You'll need to reset viewport to screen size in your main loop
}

void Renderer::DrawMeshForDepth(const Mesh &mesh, const glm::mat4 &modelMatrix)
{
    glm::mat4 mvp = m_lightSpaceMatrix * modelMatrix;

    // Set uniforms for the depth shader
    m_depthShader->set_mat4("uMVP", mvp);
    m_depthShader->set_mat4("uModel", modelMatrix);
    m_depthShader->set_int("u_isInstanced", 0);

    mesh.Bind();
    mesh.Draw();
}