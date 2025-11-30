#include <CAD_modeler/model/millingPathsDesigner/insideFiller.hpp>


std::vector<alg::Vec2> InsideFiller::Fill(const std::vector<alg::Vec2> &boundary) const
{
    int stepsToMakeU = static_cast<int>(std::floor((stopU - startU) / stepU));

    std::vector<alg::Vec2> result;

    for (int actStepU = 0; actStepU < stepsToMakeU; actStepU++) {
        const float actU = startU + stepU * actStepU;

        auto inters = IntersectionsWithUStrip(boundary, actU);

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


std::vector<alg::Vec2> InsideFiller::IntersectionsWithUStrip(const std::vector<alg::Vec2>& boundary, float uVal) const
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
                return p1.Y() > p2.Y();

            if (p1.Y() > startV && p2.Y() > startV)
                return p1.Y() > p2.Y();

            if (p1.Y() <= startV && p2.Y() > startV)
                return true;

            return false;
        }
    );

    return inters;
}


std::vector<alg::Vec2> InsideFiller::IntersectionsWithVStrip(const std::vector<alg::Vec2> &boundary, const float vVal) const
{
    LineSegment2D actSeg(startU, vVal, FirstStopU(), vVal);
    auto inters = FindIntersectionWithInBorders(boundary, actSeg);
    bool wrapU = WrapU();

    if (wrapU) {
        actSeg = LineSegment2D(0.f, vVal, stopU, vVal);

        auto newInters = FindIntersectionWithInBorders(boundary, actSeg);
        inters.insert(inters.end(), newInters.begin(), newInters.end());
    }

    std::sort(inters.begin(), inters.end(),
        [wrapU, this](const alg::Vec2& p1, const alg::Vec2& p2)
        {
            if (!wrapU || (p1.X() >= startU && p2.X() >= startU))
                return p1.X() < p2.X();

            if (p1.X() < startU && p2.X() < startU)
                return p1.X() < p2.X();

            if (p1.X() >= startU && p2.X() < startU)
                return true;

            return false;
        }
    );

    return inters;
}


std::vector<alg::Vec2> InsideFiller::FillByUThenV(const std::vector<alg::Vec2> &boundary) const
{
    int stepsToMakeV = static_cast<int>(std::floor((stopV - startV) / stepV));

    std::vector<alg::Vec2> result;

    for (int actStepV = 0; actStepV < stepsToMakeV; actStepV++) {
        const float actV = startV + stepV * actStepV;

        auto inters = IntersectionsWithVStrip(boundary, actV);

        if (inters.empty())
            continue;

        if (inters.size() != 2)
            throw std::runtime_error("InsideFiller::Fill: Invalid intersection count");

        std::vector<alg::Vec2> actStripPoints;

        const float stepsToMakeU = StepsCountU(inters[0], inters[1]);
        for (int actStepU = 0; actStepU < stepsToMakeU; actStepU++) {
            float actU = ActU(inters[0].X(), actStepU);
            actStripPoints.emplace_back(actU, actV);
        }

        if (actStepV % 2 != 0)
            std::reverse(actStripPoints.begin(), actStripPoints.end());

        result.insert(result.end(), actStripPoints.begin(), actStripPoints.end());
    }

    return result;
}


std::vector<alg::Vec2> InsideFiller::FindIntersections(const std::vector<alg::Vec2> &boundary, const LineSegment2D &seg) const
{
    std::vector<alg::Vec2> result;
    alg::Vec2 interPoint;

    for (size_t i = 1; i < boundary.size(); i++) {
        const auto& p1 = boundary[i-1];
        const auto& p2 = boundary[i];

        LineSegment2D boundarySeg(p1, p2);

        if (boundarySeg.Length() > 1.0f)
            continue;

        if (LineSegment2D::AreIntersecting(seg, boundarySeg, interPoint)) {
            result.emplace_back(interPoint);
        }
    }

    LineSegment2D lastSeg(boundary.front(), boundary.back());
    if (LineSegment2D::AreIntersecting(seg, lastSeg, interPoint))
        result.emplace_back(interPoint);

    return result;
}


std::vector<alg::Vec2> InsideFiller::FindIntersectionWithInBorders(
    const std::vector<alg::Vec2> &boundary, const LineSegment2D &seg
) const {
    std::vector<alg::Vec2> result = FindIntersections(boundary, seg);

    std::erase_if(result, [this](const alg::Vec2& p) { return !WithInBorders(p); });

    return result;
}


bool InsideFiller::WithInBorders(alg::Vec2 p) const
{
    // Check U
    if (!WrapU())
        if (p.X() < startU || p.X() > stopU)
            return false;

    if (WrapU())
        if (p.X() > stopU && p.X() < startU)
            return false;

    // Check V
    if (!WrapV())
        if (p.Y() < startV || p.Y() > stopV)
            return false;
    if (WrapV())
        if (p.Y() > stopV && p.Y() < startV)
            return false;

    return true;
}


float InsideFiller::FirstStopV() const
{
    if (WrapV())
        return 0.f;

    return stopV;
}

float InsideFiller::FirstStopU() const
{
    if (WrapU())
        return maxU;

    return stopU;
}


float InsideFiller::ActV(const float startV, const int stepNb) const
{
    float actV = startV - stepNb * stepV;

    if (actV < 0.f && WrapV())
         actV += maxV;

    return actV;
}

float InsideFiller::ActU(const float startU, const int stepNb) const
{
    float actU = startU + stepNb * stepU;

    if (actU > maxU && WrapU())
        actU -= maxU;

    return actU;
}


int InsideFiller::StepsCountV(alg::Vec2 start, alg::Vec2 stop) const
{
    if (start.Y() > stop.Y())
        return static_cast<int>(std::floor(Distance(start, stop) / stepV));

    float dist = start.Y() + maxV - stop.Y();
    return static_cast<int>(std::floor(dist / stepV));
}


int InsideFiller::StepsCountU(alg::Vec2 start, alg::Vec2 stop) const
{
    if (start.X() < stop.X())
        return static_cast<int>(std::floor(Distance(start, stop) / stepU));

    float dist = maxU - start.X() + stop.X();
    return static_cast<int>(std::floor(dist / stepU));
}


