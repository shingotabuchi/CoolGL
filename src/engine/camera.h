#pragma once

#include "component.h"
#include <glm/glm.hpp>

// Forward declarations
class Transform;

class Camera : public Component
{
public:
    enum class ProjectionType
    {
        Perspective,
        Orthographic
    };

    // Public properties (Unity-like simplicity)
    float field_of_view_degrees = 45.0f; // vertical FOV
    float near_clip = 0.1f;
    float far_clip = 2000.0f;
    float aspect_ratio = 1.0f;                                    // manual aspect when not syncing to window
    bool sync_aspect_with_window = true;                          // when true, derive aspect from current viewport
    ProjectionType projection_type = ProjectionType::Perspective; // default perspective
    // Orthographic height represented as half-extent in world units (like Unity's size)
    float orthographic_size = 5.0f;

    // Matrices derived from the owning GameObject's Transform
    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;
    glm::vec3 Position() const;

    void OnAttach() override;
    void OnDetach() override;

    std::unique_ptr<Component> Clone() const override
    {
        auto copy = std::make_unique<Camera>();
        copy->field_of_view_degrees = field_of_view_degrees;
        copy->near_clip = near_clip;
        copy->far_clip = far_clip;
        copy->aspect_ratio = aspect_ratio;
        copy->sync_aspect_with_window = sync_aspect_with_window;
        copy->projection_type = projection_type;
        copy->orthographic_size = orthographic_size;
        return copy;
    }

private:
    mutable Transform *cached_transform_ = nullptr;
};
