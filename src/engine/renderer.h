#pragma once

#include <glm/glm.hpp>
#include "mesh.h"
#include "shader.h"
#include "light.h"

class Renderer
{
public:
    bool use_shadows = false;

    // Shadow quality settings
    struct ShadowSettings
    {
        float bias = 0.005f;
        int pcf_samples = 9;
        int shadow_map_size = 2048;
        bool use_advanced_shadows = false;
        bool use_contact_hardening = false;
    } shadow_settings;

    Renderer();

    void BeginFrame(float r, float g, float b, float a);
    void UpdateLightState(int light_count, const glm::vec3 *light_dirs, const glm::vec3 *light_colors, const glm::vec3 cam_pos);
    void DrawMesh(const Mesh &mesh,
                  const Shader &shader);

    // Helper to draw only a mesh with a shader when no lighting is needed
    void DrawSimple(const Mesh &mesh,
                    const Shader &shader)
    {
        shader.use();
        mesh.Bind();
        mesh.Draw();
    }

    void InitializeShadowMap(int width, int height);
    void SetViewport(int width, int height);
    void BeginShadowPass(const glm::mat4 &lightSpaceMatrix, const glm::vec3 &lightDir);
    void EndShadowPass();
    void DrawMeshForDepth(const Mesh &mesh, const glm::mat4 &mvp);
    GLuint GetShadowMapTexture() const { return m_shadowMapTexture; }

private:
    struct CachedLightState
    {
        int count = -1;
        glm::vec3 dirs[Light::kMaxLights];
        glm::vec3 colors[Light::kMaxLights];
        glm::vec3 cam_pos;
        bool has_changed = false;
    };

    static CachedLightState s_cached_light_state_;

    GLuint m_shadowMapFBO = 0;
    GLuint m_shadowMapTexture = 0;
    int m_shadowMapWidth = 0;
    int m_shadowMapHeight = 0;
    std::unique_ptr<Shader> m_depthShader;
    glm::mat4 m_lightSpaceMatrix;
    glm::vec3 m_currentLightDir;
};
