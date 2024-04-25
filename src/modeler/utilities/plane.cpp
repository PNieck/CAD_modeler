#include <CAD_modeler/utilities/plane.hpp>


std::optional<alg::Vec3> Plane::Intersect(const Line& line)
{
    float dotWithDir = alg::Dot(perpendicularVec, line.GetDirection());

    if (dotWithDir == 0.0f) {
        return std::nullopt;
    }

    float dotWithLinePt = alg::Dot(perpendicularVec, line.GetSamplePoint());
    float dotWithPlanePt = alg::Dot(perpendicularVec, pointOnPlane);

    float t = (dotWithPlanePt - dotWithLinePt) / dotWithDir;

    return { line.GetPointOnLine(t) };
}
