#pragma once

#include <glm/glm.hpp>

struct Triangle {
    glm::vec3 vertices[3];
    glm::vec3 normals[3];

    Triangle() = default;
    Triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
        : vertices{v0, v1, v2} {}

    Triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2)
        : vertices{v0, v1, v2}, normals{n0, n1, n2} {}
};