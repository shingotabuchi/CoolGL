#pragma once

#include "component.h"
#include <glm/glm.hpp>

class Transform : public Component
{
public:
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation_euler{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    glm::mat4 LocalToWorld() const;

    std::unique_ptr<Component> Clone() const override
    {
        auto copy = std::make_unique<Transform>();
        copy->position = position;
        copy->rotation_euler = rotation_euler;
        copy->scale = scale;
        return copy;
    }
};


