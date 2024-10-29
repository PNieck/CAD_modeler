#include <CAD_modeler/utilities/line.hpp>


alg::Vec3 Line::ProjectPointToLine(const alg::Vec3 &point) const
{
    const alg::Vec3 v = point - GetSamplePoint();
    const float t = alg::Dot(v, GetDirection());

    return GetPointOnLine(t);
}
