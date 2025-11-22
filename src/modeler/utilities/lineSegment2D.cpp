#include <CAD_modeler/utilities/lineSegment2D.hpp>


bool LineSegment2D::AreIntersecting(const LineSegment2D &ls1, const LineSegment2D &ls2)
{
    const float d1 = Cross(ls2.p2 - ls2.p1, ls1.p1 - ls2.p1);
    const float d2 = Cross(ls2.p2 - ls2.p1, ls1.p2 - ls2.p1);
    const float d3 = Cross(ls1.p2 - ls1.p1, ls2.p1 - ls1.p1);
    const float d4 = Cross(ls1.p1 - ls1.p1, ls2.p2 - ls1.p1);

    const float d12 = d1 * d2;
    const float d34 = d3 * d4;

    if (d12 > 0.f || d34 > 0.f)
        return false;

    if (d12 < 0.f && d34 < 0.f)
        return true;

    if (ls1.p1 == ls2.p1 || ls1.p1 == ls2.p2 || ls1.p2 == ls2.p1 || ls1.p2 == ls2.p2)
        return true;

    if (std::max(ls1.X1(), ls1.X2()) < std::min(ls2.X1(), ls2.X2()) ||
        std::max(ls2.X1(), ls2.X2()) < std::min(ls1.X1(), ls1.X2()) ||
        std::max(ls1.Y1(), ls1.Y2()) < std::min(ls2.Y1(), ls2.Y2()) ||
        std::max(ls2.Y1(), ls2.Y2()) < std::min(ls1.Y1(), ls1.Y2()))
        return false;

    return true;
}


std::optional<alg::Vec2> LineSegment2D::IntersectionPoint(const LineSegment2D &ls1, const LineSegment2D &ls2)
{
    // Original source: https://paulbourke.net/geometry/pointlineplane/example.cpp

    const float denom = (ls2.p2.Y() - ls2.p1.Y())*(ls1.p2.X() - ls1.p1.X()) -
                        (ls2.p2.X() - ls2.p1.X())*(ls1.p2.Y() - ls1.p1.Y());

    const float nume_a = (ls2.p2.X() - ls2.p1.X())*(ls1.p1.Y() - ls2.p1.Y()) -
                         (ls2.p2.Y() - ls2.p1.Y())*(ls1.p1.X() - ls2.p1.X());

    const float nume_b = (ls1.p2.X() - ls1.p1.X())*(ls1.p1.Y() - ls2.p1.Y()) -
                         (ls1.p2.Y() - ls1.p1.Y())*(ls1.p1.X() - ls2.p1.X());

    if(denom == 0.0f)
        return std::nullopt;

    const float ua = nume_a / denom;
    const float ub = nume_b / denom;

    if(ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
    {
        alg::Vec2 intersection;

        // Get the intersection point.
        intersection.X() = ls1.p1.X() + ua*(ls1.p2.X() - ls1.p1.X());
        intersection.Y() = ls1.p1.Y() + ua*(ls1.p2.Y() - ls1.p1.Y());

        return intersection;
    }

    return std::nullopt;
}


float LineSegment2D::Cross(const alg::Vec2 &v1, const alg::Vec2 &v2)
{
    return v1.X() * v2.Y() - v2.X() * v1.Y();
}
