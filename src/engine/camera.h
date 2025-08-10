#pragma once

#include "component.h"
#include <glm/glm.hpp>

// Forward declarations
class Transform;

class Camera : public Component
{
public:
    // Public properties (Unity-like simplicity)
    float field_of_view_degrees = 45.0f;   // vertical FOV
    float near_clip = 0.1f;
    float far_clip = 100.0f;
    float aspect_ratio = 1.0f;             // should be kept in sync with window size

    // Matrices derived from the owning GameObject's Transform
    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

    void OnAttach() override;
    void OnDetach() override;

    std::unique_ptr<Component> Clone() const override
    {
        auto copy = std::make_unique<Camera>();
        copy->field_of_view_degrees = field_of_view_degrees;
        copy->near_clip = near_clip;
        copy->far_clip = far_clip;
        copy->aspect_ratio = aspect_ratio;
        return copy;
    }

private:
    mutable Transform* cached_transform_ = nullptr;
};


