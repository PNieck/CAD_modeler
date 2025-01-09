#pragma once

#include <optional>

#include <algebra/vec3.hpp>

#include "line.hpp"


class Plane {
public:
    Plane(const alg::Vec3& pointOnPlane, const alg::Vec3& perpendicularVector):
        perpendicularVec(perpendicularVector.Normalize()), pointOnPlane(pointOnPlane) {}

    [[nodiscard]]
    std::optional<alg::Vec3> Intersect(const Line& line) const;

private:
    alg::Vec3 perpendicularVec;
    alg::Vec3 pointOnPlane;
};
