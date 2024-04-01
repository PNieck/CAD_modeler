#include <CAD_modeler/utilities/plane.hpp>


std::optional<glm::vec3> Plane::Intersect(const Line & line)
{
    float dotWithDir = glm::dot(perpendicularVec, line.GetDirection());

    if (dotWithDir == 0.0f) {
        return std::nullopt;
    }

    float dotWithLinePt = glm::dot(perpendicularVec, line.GetSamplePoint());
    float dotWithPlanePt = glm::dot(perpendicularVec, pointOnPlane);

    float t = (dotWithPlanePt - dotWithLinePt) / dotWithDir;

    return { line.GetPointOnLine(t) };
}
