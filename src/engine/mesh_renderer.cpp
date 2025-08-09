#include "mesh_renderer.h"
#include "renderer.h"
#include "game_object.h"
#include "transform.h"

void MeshRenderer::OnRender(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view)
{
    if (!cached_transform_ && Owner())
    {
        cached_transform_ = Owner()->GetComponent<Transform>();
    }
    const Transform* t = cached_transform_;
    glm::mat4 model = t ? t->LocalToWorld() : glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;
    renderer.DrawMesh(mesh_, shader_, mvp, light_pos, light_color);
}


