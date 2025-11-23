#pragma once
#include <vector>

#include "../../utilities/lineSegment2D.hpp"

#include "algebra/vec2.hpp"


class InsideFiller {
public:
    InsideFiller(const float startU, const float stopU, float startV, float stopV, const float stepU, const float stepV, float maxU, float maxV):
        startU(startU), stopU(stopU), startV(startV), stopV(stopV), stepU(stepU), stepV(stepV), maxU(maxU), maxV(maxV) {}

    std::vector<alg::Vec2> Fill(const std::vector<alg::Vec2>& boundary) const;

    std::vector<alg::Vec2> IntersectionsWithStrip(const std::vector<alg::Vec2>& boundary, float uVal) const;;

private:
    float startU, stopU;
    float startV, stopV;
    float stepU, stepV;

    float maxU, maxV;

    bool WrapU() const
        { return startU < stopU; }

    bool WrapV() const
        { return startV < stopV; }

    static std::vector<alg::Vec2> FindIntersections(const std::vector<alg::Vec2>& boundary, const LineSegment2D& sqg);

    float FirstStopV() const;

    float ActV(float startV, int stepNb) const;

    int StepsCountV(alg::Vec2 p1, alg::Vec2 p2) const;
};
