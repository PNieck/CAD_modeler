#pragma once

#include <glm/glm.hpp>


class Line {
public:
    static inline Line FromTwoPoints(const glm::vec3& point1, const glm::vec3& point2)
        { return Line(point1, point1 - point2); }

    Line(const glm::vec3& pointOnLine, const glm::vec3& direction):
        pointOnLine(pointOnLine), direction(glm::normalize(direction)) {}

    inline glm::vec3 GetDirection() const { return direction; }

    inline glm::vec3 GetSamplePoint() const { return pointOnLine; }

    inline glm::vec3 GetPointOnLine(float t) const { return t * direction + pointOnLine; }

private:
    glm::vec3 pointOnLine;
    glm::vec3 direction;
};
