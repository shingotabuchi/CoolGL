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

glm::mat4 Light::GetLightSpaceMatrix() const
{
    // Define the bounds of the light's view frustum
    // These values should be tuned to tightly fit your scene
    float near_plane = 1.0f, far_plane = 50.0f;
    float frustum_size = 15.0f;
    glm::mat4 lightProjection = glm::ortho(-frustum_size, frustum_size, -frustum_size, frustum_size, near_plane, far_plane);

    // Light's position should be somewhere "behind" the scene looking towards the center
    // The position is derived from its direction.
    glm::vec3 lightPos = -WorldDirection() * 20.0f; // Move light back along its direction
    glm::vec3 target = glm::vec3(0.0f);             // Looking at the world origin
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Check for gimbal lock case (light pointing straight up/down)
    if (glm::abs(glm::dot(WorldDirection(), up)) > 0.99f)
    {
        up = glm::vec3(1.0f, 0.0f, 0.0f); // Use X-axis as up vector
    }

    glm::mat4 lightView = glm::lookAt(lightPos, target, up);

    return lightProjection * lightView;
}

void Light::OnAttach()
{
    cached_transform_ = nullptr;
    if (Owner() && Owner()->GetScene())
    {
        Owner()->GetScene()->RegisterLight(this);
    }
}

void Light::OnDetach()
{
    if (Owner() && Owner()->GetScene())
    {
        Owner()->GetScene()->UnregisterLight(this);
    }
}
