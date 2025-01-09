#include <CAD_modeler/utilities/plane.hpp>


std::optional<alg::Vec3> Plane::Intersect(const Line& line) const
{
    const float dotWithDir = Dot(perpendicularVec, line.GetDirection());

    if (dotWithDir == 0.0f) {
        return std::nullopt;
    }

    const float dotWithLinePt = Dot(perpendicularVec, line.GetSamplePoint());
    const float dotWithPlanePt = Dot(perpendicularVec, pointOnPlane);

    const float t = (dotWithPlanePt - dotWithLinePt) / dotWithDir;

    return { line.GetPointOnLine(t) };
}
