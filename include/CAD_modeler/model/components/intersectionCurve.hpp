#pragma once

#include <algebra/vec4.hpp>

#include <ecs/eventsManager.hpp>

#include <vector>
#include <deque>


class IntersectionPoint {
public:
    IntersectionPoint(const float u1, const float v1, const float u2, const float v2):
        vec(u1, v1, u2, v2) {}

    explicit IntersectionPoint(const alg::Vec4& vec):
        vec(vec) {}

    IntersectionPoint():
        IntersectionPoint(0.f, 0.f, 0.f, 0.f) {}

    float& U1()
        { return vec.X(); }

    float& V1()
        { return vec.Y(); }

    float& U2()
        { return vec.Z(); }

    float& V2()
        { return vec.W(); }

    [[nodiscard]]
    float U1() const
        { return vec.X(); }

    [[nodiscard]]
    float V1() const
        { return vec.Y(); }

    [[nodiscard]]
    float U2() const
        { return vec.Z(); }

    [[nodiscard]]
    float V2() const
        { return vec.W(); }


    alg::Vec4& AsVector()
        { return vec; }

    [[nodiscard]]
    const alg::Vec4& AsVector() const
        { return vec; }

private:
    alg::Vec4 vec;
};


class IntersectionCurve {
public:
    IntersectionCurve(std::vector<IntersectionPoint>&& points, const bool isOpen, const HandlerId deletionHandler):
        isOpen(isOpen), intersectionPoints(std::move(points)), deletionHandler(deletionHandler) {}

    IntersectionCurve(const std::deque<IntersectionPoint>& points, const bool isOpen, const HandlerId deletionHandler):
        isOpen(isOpen), intersectionPoints(points.begin(), points.end()), deletionHandler(deletionHandler) {}

    [[nodiscard]]
    size_t Size() const
         { return intersectionPoints.size(); }

    const IntersectionPoint& operator[](const size_t idx) const {
        return intersectionPoints.at(idx);
    }

    bool isOpen;
    std::vector<IntersectionPoint> intersectionPoints;
    HandlerId deletionHandler;
};
