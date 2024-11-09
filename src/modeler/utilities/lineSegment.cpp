#include <CAD_modeler/utilities/lineSegment.hpp>

#include <algorithm>


alg::Vec3 LineSegment::NearestPoint(const alg::Vec3 &point) const
{
    const alg::Vec3 v = point - startingPoint;
    const float t = std::clamp(alg::Dot(v, direction), 0.f, maxT);

    return startingPoint + direction * t;
}
