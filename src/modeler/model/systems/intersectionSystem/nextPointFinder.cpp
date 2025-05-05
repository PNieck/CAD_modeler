#include <CAD_modeler/model/systems/intersectionSystem/nextPointFinder.hpp>

#include <rootFinding/newtonMethod.hpp>

#include "CAD_modeler/model/systems/intersectionsSystem.hpp"

#include "CAD_modeler/utilities/angle.hpp"


using namespace interSys;


class NextPointDistFun final : public root::FunctionToFindRoot {
public:
    NextPointDistFun(Surface& s1, Surface& s2, const alg::Vec3& prevPoint,
                     const alg::Vec3& tangent, const float step):
        surface1(s1), surface2(s2), prevPoint(prevPoint), tangent(tangent), step(step) {}

    alg::Vec4 Value(alg::Vec4 args) override {
        if (!PointInDomains(surface1, surface2, IntersectionPoint(args))) {
            outsideDomain = true;
            return alg::Vec4(NAN);
        }

        const auto point1 = surface1.PointOnSurface(args.X(), args.Y());
        const auto point2 = surface2.PointOnSurface(args.Z(), args.W());

        auto diff = point1 - point2;
        auto v = Dot(point1 - prevPoint, tangent) - step;

        return {
            diff.X(),
            diff.Y(),
            diff.Z(),
            v
        };
    }

    alg::Mat4x4 Jacobian(alg::Vec4 args) override {
        const auto derE1X = surface1.PartialDerivativeU(args.X(), args.Y());
        const auto derE1Y = surface1.PartialDerivativeV(args.X(), args.Y());

        const auto derE2Z = surface2.PartialDerivativeU(args.Z(), args.W());
        const auto derE2W = surface2.PartialDerivativeV(args.Z(), args.W());

        const float partX = Dot(tangent, derE1X);
        const float partY = Dot(tangent, derE1Y);

        return {
            derE1X.X(), derE1Y.X(), -derE2Z.X(), -derE2W.X(),
            derE1X.Y(), derE1Y.Y(), -derE2Z.Y(), -derE2W.Y(),
            derE1X.Z(), derE1Y.Z(), -derE2Z.Z(), -derE2W.Z(),
                 partX,      partY,         0.f,         0.f
        };
    }

    [[nodiscard]]
    bool WasEvaluatedOutsideTheDomain() const
        { return outsideDomain; }

private:
    Surface& surface1;
    Surface& surface2;

    alg::Vec3 prevPoint;
    alg::Vec3 tangent;
    float step;

    bool outsideDomain = false;
};


NextPointFinder::NextPointFinder(Surface &s1, Surface &s2, const IntersectionPoint &firstPoint, const float step):
    surface1(s1), surface2(s2), actTangent(Tangent(firstPoint)), actPoint(firstPoint), step(step)
{
}


bool NextPointFinder::FindNext()
{
    if (wasLastPoint)
        return false;

    float actStep = step;
    float remainingDist = step;
    const float minStep = step / 1024.f;
    std::optional<alg::Vec4> nextPoint;

    do {
        alg::Vec3 actSurfacePoint = surface1.PointOnSurface(actPoint.U1(), actPoint.V1());

        NextPointDistFun fun(
            surface1, surface2,
            actSurfacePoint,
            actTangent,
            actStep
        );

        nextPoint = NewtonMethod(fun, actPoint.AsVector(), 1e-5);
        if (!nextPoint.has_value()) {
            actStep /= 2.f;
            if (actStep < minStep)
                return false;
        }
        else if (fun.WasEvaluatedOutsideTheDomain()) {
            wasLastPoint = true;
            actStep /= 2.f;
            remainingDist /= 2.f;
        }
        else {
            remainingDist -= actStep;
            actStep = remainingDist;

            actPoint = IntersectionPoint(nextPoint.value());
            UpdateTangent();
        }

    } while (remainingDist > 0.f);

    return true;
}


alg::Vec3 NextPointFinder::Tangent(const IntersectionPoint &point) const
{
    const alg::Vec3 normal1 = surface1.NormalVector(point.U1(), point.V1());
    const alg::Vec3 normal2 = surface2.NormalVector(point.U2(), point.V2());

    return Cross(normal1, normal2).Normalize();
}


bool NextPointFinder::ReverseTangent(const alg::Vec3 &newTangent) const
{
    return Angle::FromRadians(std::acos(Dot(newTangent, actTangent))).ToDegrees() >= 120.f;
}


void NextPointFinder::UpdateTangent() {
    alg::Vec3 newTangent = Tangent(actPoint);

    if (ReverseTangent(newTangent))
        newTangent = -newTangent;

    actTangent = newTangent;
}
