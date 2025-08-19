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

    struct CachedLightState
    {
        int count = -1;
        glm::vec3 dirs[Light::kMaxLights];
        glm::vec3 colors[Light::kMaxLights];
        GLuint program = 0;
    };

    Renderer();

    void BeginFrame(float r, float g, float b, float a);
    void DrawMesh(const Mesh &mesh,
                  const Shader &shader,
                  CachedLightState &light_state,
                  const glm::mat4 &mvp,
                  int light_count,
                  const glm::vec3 *light_dirs_object_space,
                  const glm::vec3 *light_colors);

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
    std::unique_ptr<Shader> _depthShader;
};
