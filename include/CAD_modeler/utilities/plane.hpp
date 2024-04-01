#pragma once

#include <optional>

#include <glm/vec3.hpp>

#include "line.hpp"


class Plane {
public:
    Plane(const glm::vec3& pointOnPlane, const glm::vec3& perpendicularVector):
        perpendicularVec(glm::normalize(perpendicularVector)), pointOnPlane(pointOnPlane) {}

    std::optional<glm::vec3> Intersect(const Line& line);

private:
    glm::vec3 perpendicularVec;
    glm::vec3 pointOnPlane;
};
