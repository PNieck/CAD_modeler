#pragma once
#include <vector>

#include "../../utilities/lineSegment2D.hpp"

#include "algebra/vec2.hpp"


class InsideFiller {
public:
    InsideFiller(const float startU, const float stopU, float startV, float stopV, const float stepU, const float stepV, float maxU, float maxV):
        startU(startU), stopU(stopU), startV(startV), stopV(stopV), stepU(stepU), stepV(stepV), maxU(maxU), maxV(maxV) {}

    [[nodiscard]]
    std::vector<alg::Vec2> Fill(const std::vector<alg::Vec2>& boundary) const;

    [[nodiscard]]
    std::vector<alg::Vec2> IntersectionsWithUStrip(const std::vector<alg::Vec2>& boundary, float uVal) const;;

    [[nodiscard]]
    std::vector<alg::Vec2> IntersectionsWithVStrip(const std::vector<alg::Vec2>& boundary, float vVal) const;

    [[nodiscard]]
    std::vector<alg::Vec2> FillByUThenV(const std::vector<alg::Vec2>& boundary) const;

private:
    float startU, stopU;
    float startV, stopV;
    float stepU, stepV;

    float maxU, maxV;

    [[nodiscard]]
    bool WrapU() const
        { return startU > stopU; }

    [[nodiscard]]
    bool WrapV() const
        { return startV < stopV; }

    [[nodiscard]]
    std::vector<alg::Vec2> FindIntersections(const std::vector<alg::Vec2>& boundary, const LineSegment2D& seg) const;
    [[nodiscard]]
    std::vector<alg::Vec2> FindIntersectionWithInBorders(const std::vector<alg::Vec2>& boundary, const LineSegment2D& seg) const;

    [[nodiscard]]
    bool WithInBorders(alg::Vec2 p) const;

    [[nodiscard]]
    float FirstStopV() const;

    [[nodiscard]]
    float FirstStopU() const;

    [[nodiscard]]
    float ActV(float startV, int stepNb) const;

    [[nodiscard]]
    float ActU(float startU, int stepNb) const;

    [[nodiscard]]
    int StepsCountV(alg::Vec2 start, alg::Vec2 stop) const;

    [[nodiscard]]
    int StepsCountU(alg::Vec2 start, alg::Vec2 stop) const;
};
