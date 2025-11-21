#pragma once

#include <algebra/vec2.hpp>

#include <optional>


class LineSegment2D {
public:
    LineSegment2D(const float x1, const float y1, const float x2, const float y2):
        p1(x1, y1), p2(x2, y2) {}

    LineSegment2D(const alg::Vec2& p1, const alg::Vec2& p2):
        p1(p1), p2(p2) {}

    alg::Vec2 p1, p2;

    [[nodiscard]]
    float X1() const
        { return p1.X(); }

    [[nodiscard]]
    float Y1() const
        { return p1.Y(); }

    [[nodiscard]]
    float X2() const
        { return p2.X(); }

    [[nodiscard]]
    float Y2() const
        { return p2.Y(); }

    static bool AreIntersecting(const LineSegment2D& ls1, const LineSegment2D& ls2);

    static std::optional<alg::Vec2> IntersectionPoint(const LineSegment2D& ls1, const LineSegment2D& ls2);

private:
    static float Cross(const alg::Vec2& v1, const alg::Vec2& v2);
};
