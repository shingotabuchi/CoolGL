#include "camera.h"
#include "game_object.h"
#include "scene.h"
#include "transform.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::ViewMatrix() const
{
    if (!cached_transform_ && Owner())
    {
        cached_transform_ = Owner()->GetComponent<Transform>();
    }
    const Transform* t = cached_transform_;
    if (!t)
    {
        return glm::mat4(1.0f);
    }

    // Build a view matrix from the transform's position and Euler rotation.
    // We assume the Transform's LocalToWorld uses XYZ Euler rotations and scale.
    // The camera looks along -Z in its local space.
    glm::mat4 world = t->LocalToWorld();
    // View is inverse of world transform
    return glm::inverse(world);
}

glm::mat4 Camera::ProjectionMatrix() const
{
    return glm::perspective(glm::radians(field_of_view_degrees), aspect_ratio, near_clip, far_clip);
}

void Camera::OnAttach()
{
    cached_transform_ = nullptr;
    if (Owner() && Owner()->SceneContext())
    {
        Owner()->SceneContext()->RegisterCamera(this);
    }
}

void Camera::OnDetach()
{
    if (Owner() && Owner()->SceneContext())
    {
        Owner()->SceneContext()->UnregisterCamera(this);
    }
}


