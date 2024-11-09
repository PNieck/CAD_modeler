#pragma once

#include <algebra/vec3.hpp>


class Line {
public:
    static inline Line FromTwoPoints(const alg::Vec3& point1, const alg::Vec3& point2)
        { return { point1, point2 - point1 }; }

    Line(const alg::Vec3& pointOnLine, const alg::Vec3& direction):
        pointOnLine(pointOnLine), direction(direction.Normalize()) {}

    inline alg::Vec3 GetDirection() const { return direction; }

    inline alg::Vec3 GetSamplePoint() const { return pointOnLine; }

    inline alg::Vec3 GetPointOnLine(const float t) const
        { return t * direction + pointOnLine; }

    alg::Vec3 ProjectPointToLine(const alg::Vec3& point) const;

private:
    alg::Vec3 pointOnLine;
    alg::Vec3 direction;
};
