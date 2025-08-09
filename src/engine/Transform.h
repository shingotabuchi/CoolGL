#pragma once

#include "Component.h"
#include <glm/glm.hpp>

class Transform : public Component
{
public:
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotationEuler{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    glm::mat4 localToWorld() const;
};


