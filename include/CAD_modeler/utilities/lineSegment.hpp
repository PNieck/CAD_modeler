#pragma once

#include <algebra/vec3.hpp>


class LineSegment {
public:
    LineSegment(const alg::Vec3& startPoint, const alg::Vec3& endPoint):
        startingPoint(startPoint), direction((endPoint - startPoint).Normalize()), maxT((endPoint - startPoint).Length()) {}

    alg::Vec3 NearestPoint(const alg::Vec3& point) const;

private:
    alg::Vec3 startingPoint;
    alg::Vec3 direction;

    float maxT;
};
