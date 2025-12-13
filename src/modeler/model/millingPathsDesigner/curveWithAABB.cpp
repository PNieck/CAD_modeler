#include <CAD_modeler/model/millingPathsDesigner/curveWithAABB.hpp>

#include "CAD_modeler/utilities/lineSegment2D.hpp"


CurveWithAABB::CurveWithAABB(const std::vector<alg::Vec2> &points, float borderV, float offsetV):
    points(points.size()),
    maxU(-std::numeric_limits<float>::infinity()),
    maxV(-std::numeric_limits<float>::infinity()),
    minU(std::numeric_limits<float>::infinity()),
    minV(std::numeric_limits<float>::infinity()),
    borderV(borderV), offsetV(offsetV)
{
    for (size_t i = 0; i < points.size(); ++i) {
        auto& p = this->points[i];
        p = points[i];

        if (p.Y() < borderV)
            this->points[i].Y() += offsetV;

        if (p.X() > maxU)
            maxU = p.X();

        if (p.X() < minU)
            minU = p.X();

        if (p.Y() > maxV)
            maxV = p.Y();

        if (p.Y() < minV)
            minV = p.Y();
    }
}


bool CurveWithAABB::Inside(const float u, float v) const
{
    if (v < borderV)
        v += offsetV;

    if (u < minU || u > maxU || v < minV || v > maxV)
        return false;

    const LineSegment2D seg(u, v, u, borderV);

    int intersCnt = 0;
    for (size_t i = 1; i < points.size(); ++i) {
        auto const& p1 = points[i-1];
        auto const& p2 = points[i];

        LineSegment2D curveSeg(p1, p2);
        if (LineSegment2D::AreIntersecting(seg, curveSeg))
            intersCnt++;
    }

    return intersCnt % 2 == 1;
}
