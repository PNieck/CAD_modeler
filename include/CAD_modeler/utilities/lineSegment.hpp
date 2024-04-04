#pragma once

#include <glm/glm.hpp>


class LineSegment {
public:
    LineSegment(const glm::vec& startingPoint, const glm::vec3 direction):
        startingPoint(startingPoint), direction(glm::normalize(direction)) {}

private:
    glm::vec3 startingPoint;
    glm::vec3 direction;
};
