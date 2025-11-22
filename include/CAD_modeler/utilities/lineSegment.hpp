#pragma once

#include <algebra/vec3.hpp>


class LineSegment {
public:
    LineSegment(const alg::Vec3& startPoint, const alg::Vec3& endPoint):
        startingPoint(startPoint), direction((endPoint - startPoint).Normalize()), maxT((endPoint - startPoint).Length()) {}

    [[nodiscard]]
    alg::Vec3 NearestPoint(const alg::Vec3& point) const;

    [[nodiscard]]
    const alg::Vec3& P1() const
        { return startingPoint; }

    [[nodiscard]]
    alg::Vec3 P2() const
        { return startingPoint + direction * maxT; }

private:
    alg::Vec3 startingPoint;
    alg::Vec3 direction;

    float maxT;
};
