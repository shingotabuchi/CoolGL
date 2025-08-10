#pragma once

#include <glm/glm.hpp>
#include "mesh.h"
#include "shader.h"

class Renderer
{
public:
    Renderer();

    void BeginFrame(float r, float g, float b, float a);
    void DrawMesh(const Mesh& mesh,
                  const Shader& shader,
                  const glm::mat4& mvp,
                  int light_count,
                  const glm::vec3* light_dirs_object_space,
                  const glm::vec3* light_colors);

private:
    // Cached GL state to avoid redundant uniform sets/binds per frame
    struct CachedLightState {
        int count = -1;
        glm::vec3 dirs[8];
        glm::vec3 colors[8];
        GLuint program = 0; // which shader these were for
    };
    thread_local static CachedLightState s_cachedLightState;
};


