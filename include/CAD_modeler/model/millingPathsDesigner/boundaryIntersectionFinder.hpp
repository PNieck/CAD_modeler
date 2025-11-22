#pragma once

#include <vector>

#include "../components/position.hpp"
#include "../../utilities/lineSegment2D.hpp"


class BoundaryIntersectionFinder {
public:
    explicit BoundaryIntersectionFinder(const std::vector<Position>& boundary);

    Position Intersection(const LineSegment2D& segment);
    Position Intersection(float xValue);

    //Position GoAlongBoundary(float dist);

private:
    const std::vector<Position>& boundary;
    size_t actIdx = 0;

    //alg::Vec2 prevIntersectionPoint;

    LineSegment2D ActSegment() const;
};
