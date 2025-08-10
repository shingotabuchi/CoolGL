#pragma once

#include "component.h"
#include <glm/glm.hpp>

// Forward declarations
class Transform;

// Simple directional light component (Unity-like)
// Uses the owning GameObject's Transform rotation to determine light direction.
// The light direction is defined as the incoming light direction in world space
// (i.e., direction the light rays travel towards the scene).
class Light : public Component
{
public:
    static constexpr int kMaxLights = 4;
    glm::vec3 color{1.0f, 1.0f, 1.0f};
    float intensity{1.0f};

    // Returns normalized incoming light direction in world space.
    glm::vec3 WorldDirection() const;

    void OnAttach() override;
    void OnDetach() override;

private:
    mutable Transform* cached_transform_ = nullptr;
};


