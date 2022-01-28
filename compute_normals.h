#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

// computes the normal of a triangle given its vertices, counter-clockwise
static glm::vec3 computeNormal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
    glm::vec3 diff1 = v1 - v0;
    glm::vec3 diff2 = v2 - v0;
    glm::vec3 normal = glm::normalize(glm::cross(diff1, diff2));
    return normal;
}
