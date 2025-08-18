#include "transform.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Transform::LocalToWorld() const
{
    glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::rotate(m, glm::radians(rotation_euler.x), glm::vec3(1, 0, 0));
    m = glm::rotate(m, glm::radians(rotation_euler.y), glm::vec3(0, 1, 0));
    m = glm::rotate(m, glm::radians(rotation_euler.z), glm::vec3(0, 0, 1));
    m = glm::scale(m, scale);
    return m;
}


