#include "camera.h"
#include "game_object.h"
#include "scene.h"
#include "transform.h"
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::ViewMatrix() const
{
    if (!cached_transform_ && Owner())
    {
        cached_transform_ = Owner()->GetComponent<Transform>();
    }
    const Transform *t = cached_transform_;
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
    float ar = aspect_ratio;
    if (sync_aspect_with_window)
    {
        // Query current OpenGL viewport to derive aspect. This keeps cameras in sync with window size
        // without tight coupling.
        GLint vp[4] = {0, 0, 1, 1};
        glGetIntegerv(GL_VIEWPORT, vp);
        const float w = static_cast<float>(vp[2]);
        const float h = static_cast<float>(vp[3]);
        if (w > 0.0f && h > 0.0f)
        {
            ar = w / h;
        }
    }
    if (projection_type == ProjectionType::Perspective)
    {
        return glm::perspective(glm::radians(field_of_view_degrees), ar, near_clip, far_clip);
    }
    else
    {
        // Orthographic projection: symmetrical bounds
        const float halfHeight = orthographic_size;
        const float halfWidth = halfHeight * ar;
        const float left = -halfWidth;
        const float right = halfWidth;
        const float bottom = -halfHeight;
        const float top = halfHeight;
        return glm::ortho(left, right, bottom, top, near_clip, far_clip);
    }
}

void Camera::OnAttach()
{
    cached_transform_ = nullptr;
    if (Owner() && Owner()->GetScene())
    {
        Owner()->GetScene()->RegisterCamera(this);
    }
}

void Camera::OnDetach()
{
    if (Owner() && Owner()->GetScene())
    {
        Owner()->GetScene()->UnregisterCamera(this);
    }
}
