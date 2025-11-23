#include <CAD_modeler/model/millingPathsDesigner/insideFiller.hpp>


std::vector<alg::Vec2> InsideFiller::Fill(const std::vector<alg::Vec2> &boundary) const
{
    int stepsToMakeU = static_cast<int>(std::floor((stopU - startU) / stepU));

    std::vector<alg::Vec2> result;

    for (int actStepU = 0; actStepU < stepsToMakeU; actStepU++) {
        const float actU = startU + stepU * actStepU;

        auto inters = IntersectionsWithStrip(boundary, actU);

        if (inters.empty())
            continue;

        if (inters.size() != 2)
            throw std::runtime_error("InsideFiller::Fill: Invalid intersection count");

        std::vector<alg::Vec2> actStripPoints;

        const float stepsToMakeV = StepsCountV(inters[0], inters[1]);
        for (int actStepV = 0; actStepV < stepsToMakeV; actStepV++) {
            float actV = ActV(inters[0].Y(), actStepV);
            actStripPoints.emplace_back(actU, actV);
        }

        if (actStepU % 2 != 0)
            std::reverse(actStripPoints.begin(), actStripPoints.end());

        result.insert(result.end(), actStripPoints.begin(), actStripPoints.end());
    }

    return result;
}


std::vector<alg::Vec2> InsideFiller::IntersectionsWithStrip(const std::vector<alg::Vec2>& boundary, float uVal) const
{
    LineSegment2D actSeg(uVal, startV, uVal, FirstStopV());
    auto inters = FindIntersections(boundary, actSeg);
    bool wrapV = WrapV();

    if (wrapV) {
        actSeg = LineSegment2D(uVal, maxV, uVal, stopU);

        auto newInters = FindIntersections(boundary, actSeg);
        inters.insert(inters.end(), newInters.begin(), newInters.end());
    }

    std::sort(inters.begin(), inters.end(),
        [wrapV, this](const alg::Vec2& p1, const alg::Vec2& p2)
        {
            if (!wrapV || (p1.Y() <= startV && p2.Y() <= startV))
                return p1.Y() < p2.Y();

            if (p1.Y() > startV && p2.Y() > startV)
                return p1.Y() > p2.Y();

            if (p1.Y() <= startV && p2.Y() > startV)
                return true;

            return false;
        }
    );

    return inters;
}


std::vector<alg::Vec2> InsideFiller::FindIntersections(const std::vector<alg::Vec2> &boundary, const LineSegment2D &sqg) {
    std::vector<alg::Vec2> result;
    alg::Vec2 interPoint;

    for (size_t i = 1; i < boundary.size(); i++) {
        const auto& p1 = boundary[i-1];
        const auto& p2 = boundary[i];

        LineSegment2D boundarySeg(p1, p2);

        if (LineSegment2D::AreIntersecting(sqg, boundarySeg, interPoint)) {
            result.emplace_back(interPoint);
        }
    }

    LineSegment2D lastSeg(boundary.front(), boundary.back());
    if (LineSegment2D::AreIntersecting(sqg, lastSeg, interPoint))
        result.emplace_back(interPoint);

    return result;
}


float InsideFiller::FirstStopV() const
{
    if (WrapV())
        return 0.f;

    return stopV;
}


float InsideFiller::ActV(float startV, int stepNb) const
{
    float actV = startV - stepNb * stepV;

    if (actV < 0.f && WrapV())
         actV += maxV;

    return actV;
}


int InsideFiller::StepsCountV(alg::Vec2 start, alg::Vec2 stop) const
{
    if (start.Y() > stop.Y())
        return static_cast<int>(std::floor(Distance(start, stop) / stepV));

    float dist = start.Y() + maxV - stop.Y();
    return static_cast<int>(std::floor(dist / stepV));
}


