#include "light.h"
#include "transform.h"
#include "game_object.h"
#include "scene.h"

#include <glm/gtc/matrix_transform.hpp>

glm::vec3 Light::WorldDirection() const
{
    if (!cached_transform_ && Owner())
    {
        cached_transform_ = Owner()->GetComponent<Transform>();
    }
    // By convention, forward is -Z in local space.
    // Transform it by the rotation part of the world matrix to get world dir.
    if (!cached_transform_)
    {
        return glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
    }
    glm::mat4 world = cached_transform_->LocalToWorld();
    // Extract rotation from world by transforming local forward vector.
    glm::vec3 localForward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 worldForward = glm::normalize(glm::vec3(world * glm::vec4(localForward, 0.0f)));
    return worldForward;
}

void Light::OnAttach()
{
    cached_transform_ = nullptr;
    if (Owner() && Owner()->SceneContext())
    {
        Owner()->SceneContext()->RegisterLight(this);
    }
}

void Light::OnDetach()
{
    if (Owner() && Owner()->SceneContext())
    {
        Owner()->SceneContext()->UnregisterLight(this);
    }
}


