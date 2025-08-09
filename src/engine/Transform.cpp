#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Transform::localToWorld() const
{
    glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::rotate(m, rotationEuler.x, glm::vec3(1, 0, 0));
    m = glm::rotate(m, rotationEuler.y, glm::vec3(0, 1, 0));
    m = glm::rotate(m, rotationEuler.z, glm::vec3(0, 0, 1));
    m = glm::scale(m, scale);
    return m;
}


