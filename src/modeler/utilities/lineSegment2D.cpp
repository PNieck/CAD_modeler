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


bool LineSegment2D::AreIntersecting(const LineSegment2D &ls1, const LineSegment2D &ls2, alg::Vec2 &intersectionPoint)
{
    if (!AreIntersecting(ls1, ls2))
        return false;

    const float a1 = ls1.p2.Y() - ls1.p1.Y();
    const float b1 = ls1.p1.X() - ls1.p2.X();
    const float c1 = a1 * ls1.p1.X() + b1 * ls1.p1.Y();

    const float a2 = ls2.p2.Y() - ls2.p1.Y();
    const float b2 = ls2.p1.X() - ls2.p2.X();
    const float c2 = a2 * ls2.p1.X() + b2 * ls2.p1.Y();

    const float det = a1 * b2 - a2 * b1;
    if(det == 0) {
        // Lines are parallel or coincident
        return false;
    }

    intersectionPoint.X() = (c1 * b2 - c2 * b1) / det;
    intersectionPoint.Y() = (a1 * c2 - a2 * c1) / det;

    return true;
}


float LineSegment2D::Cross(const alg::Vec2 &v1, const alg::Vec2 &v2)
{
    return v1.X() * v2.Y() - v2.X() * v1.Y();
}
