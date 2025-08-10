#include "mesh_renderer.h"
#include "renderer.h"
#include "game_object.h"
#include "transform.h"
#include "scene.h"
#include "light.h"

void MeshRenderer::OnRender(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view)
{
    if (!cached_transform_ && Owner())
    {
        cached_transform_ = Owner()->GetComponent<Transform>();
    }
    const Transform* t = cached_transform_;
    glm::mat4 model = t ? t->LocalToWorld() : glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;

    // Precompute inverse(model) for transforming directions to object space
    const glm::mat4 invModel = glm::inverse(model);

    // Fetch lights from scene and transform their world directions into object space
    glm::vec3 lightDirs[Light::kMaxLights];
    glm::vec3 lightColors[Light::kMaxLights];
    int lightCount = 0;
    if (Owner() && Owner()->SceneContext())
    {
        const auto& lights = Owner()->SceneContext()->GetLights();
        for (size_t i = 0; i < lights.size() && lightCount < Light::kMaxLights; ++i)
        {
            const Light* light = lights[i];
            if (!light) continue;
            const glm::vec3 worldDir = glm::normalize(light->WorldDirection());
            glm::vec3 objectDir = glm::vec3(invModel * glm::vec4(worldDir, 0.0f));
            lightDirs[lightCount] = glm::normalize(objectDir);
            lightColors[lightCount] = light->color * light->intensity;
            ++lightCount;
        }
    }
    if (lightCount == 0)
    {
        // Fallback to a default light if none present
        lightDirs[0] = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
        lightColors[0] = light_color;
        lightCount = 1;
    }

    renderer.DrawMesh(mesh_, shader_, mvp, lightCount, lightDirs, lightColors);
}


