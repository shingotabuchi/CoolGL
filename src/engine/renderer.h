#pragma once

#include <glm/glm.hpp>
#include "mesh.h"
#include "shader.h"
#include "light.h"

class Renderer
{
public:
    const static int SHADOW_MAP_WIDTH = 2048;
    const static int SHADOW_MAP_HEIGHT = 2048;

    Renderer();

    void BeginFrame(float r, float g, float b, float a);
    void UpdateLightState(int light_count, const glm::vec3 *light_dirs, const glm::vec3 *light_colors, const glm::vec3 cam_pos);
    void DrawMesh(const Mesh &mesh,
                  const Shader &shader,
                  const glm::mat4 &mvp);

    // Helper to draw only a mesh with a shader when no lighting is needed
    void DrawSimple(const Mesh &mesh,
                    const Shader &shader)
    {
        shader.use();
        mesh.Bind();
        mesh.Draw();
    }

    // void InitializeShadowMap();
    // void BeginShadowPass(const glm::mat4 &lightSpaceMatrix);
    // void EndShadowPass();
    // void DrawMeshForDepth(const Mesh &mesh, const glm::mat4 &mvp);

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

    std::unique_ptr<Shader> _depthShader;
};
