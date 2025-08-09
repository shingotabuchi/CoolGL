#include "MeshRenderer.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Transform.h"

void MeshRenderer::onRender(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view)
{
    const Transform* t = owner() ? owner()->getComponent<Transform>() : nullptr;
    glm::mat4 model = t ? t->localToWorld() : glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;
    renderer.drawMesh(mesh_, shader_, mvp, lightPos, lightColor);
}


