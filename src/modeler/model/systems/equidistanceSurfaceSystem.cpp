#include <CAD_modeler/model/systems/equidistanceSurfaceSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/equidistantSurfaceParameters.hpp>
#include <CAD_modeler/model/components/wraps.hpp>


Entity EquidistanceSurfaceSystem::AddSurface(const Entity base, const float dist)
{
    const Entity result = coordinator->CreateEntity();

    const EquidistanceSurfaceParameters params {
        .baseSurface = base,
        .distance = dist,
    };

    coordinator->AddComponent(result, params);

    if (coordinator->HasComponent<WrapU>(base))
        coordinator->AddComponent(result, WrapU());

    if (coordinator->HasComponent<WrapV>(base))
        coordinator->AddComponent(result, WrapV());

    entities.insert(result);

    return result;
}


Position EquidistanceSurfaceSystem::PointOnSurface(const Entity e, const float u, const float v) const
{
    const auto [baseSurface, distance] = coordinator->GetComponent<EquidistanceSurfaceParameters>(e);

    const Position basePos = baseSystem->PointOnSurface(baseSurface, u, v);
    const alg::Vec3 baseNormal = baseSystem->NormalVector(baseSurface, u, v);

    return basePos.vec + distance * baseNormal.Normalize();
}


alg::Vec3 EquidistanceSurfaceSystem::NormalVector(const Entity e, const float u, const float v) const
{
    const Entity baseSurface = coordinator->GetComponent<EquidistanceSurfaceParameters>(e).baseSurface;
    return baseSystem->NormalVector(baseSurface, u, v);
}


float EquidistanceSurfaceSystem::MaxU(const Entity e) const
{
    const Entity baseSurface = coordinator->GetComponent<EquidistanceSurfaceParameters>(e).baseSurface;
    return baseSystem->MaxU(baseSurface);
}


float EquidistanceSurfaceSystem::MaxV(const Entity e) const
{
    const Entity baseSurface = coordinator->GetComponent<EquidistanceSurfaceParameters>(e).baseSurface;
    return baseSystem->MaxV(baseSurface);
}

alg::Vec3 EquidistanceSurfaceSystem::PartialDerivativeU(float dist, const alg::Vec3 &partialU,
    const alg::Vec3 &partialV, const alg::Vec3 &partialUU, const alg::Vec3 &partialUV) {
    const alg::Vec3 s = Cross(partialU, partialV);
    const float sLenSq = s.LengthSquared();
    const float sLen = sqrt(sLenSq);

    const alg::Vec3 partialSU = Cross(partialUU, partialV) + Cross(partialU, partialUV);
    const float partialSLenU = Dot(s, partialSU) / sLen;

    const alg::Vec3 partialNU = (sLen * partialSU - partialSLenU * s) / sLenSq;

    return partialU + dist * partialNU;
}


alg::Vec3 EquidistanceSurfaceSystem::PartialDerivativeV(float dist, const alg::Vec3 &partialU,
    const alg::Vec3 &partialV, const alg::Vec3 &partialVV, const alg::Vec3 &partialUV) {
    const alg::Vec3 s = Cross(partialU, partialV);
    const float sLenSq = s.LengthSquared();
    const float sLen = sqrt(sLenSq);

    const alg::Vec3 partialSV = Cross(partialUV, partialV) + Cross(partialU, partialVV);

    const float partialSLenV = Dot(s, partialSV) / sLen;

    const alg::Vec3 partialNV = (sLen * partialSV - partialSLenV * s) / sLenSq;

    return partialV + dist * partialNV;
}


alg::Vec3 EquidistanceSurfaceSystem::PartialDerivativeUApprox(Entity e, float u, float v) const
{
    constexpr float eps = 1e-6f;

    if (u + eps > MaxU(e)) {
        const Position v1 = PointOnSurface(e, u, v);
        const Position v2 = PointOnSurface(e, u - eps, v);

        return (v1.vec - v2.vec) / eps;
    }

    if (u - eps < 0.f) {
        const Position v1 = PointOnSurface(e, u + eps, v);
        const Position v2 = PointOnSurface(e, u, v);

        return (v1.vec - v2.vec) / eps;
    }

    const Position v1 = PointOnSurface(e, u + eps, v);
    const Position v2 = PointOnSurface(e, u - eps, v);

    return (v1.vec - v2.vec) / (2.f * eps);
}


alg::Vec3 EquidistanceSurfaceSystem::PartialDerivativeVApprox(Entity e, float u, float v) const
{
    constexpr float eps = 1e-6f;

    if (v + eps > MaxV(e)) {
        const Position v1 = PointOnSurface(e, u, v);
        const Position v2 = PointOnSurface(e, u, v - eps);

        return (v1.vec - v2.vec) / eps;
    }

    if (v - eps < 0.f) {
        const Position v1 = PointOnSurface(e, u, v + eps);
        const Position v2 = PointOnSurface(e, u, v);

        return (v1.vec - v2.vec) / eps;
    }

    const Position v1 = PointOnSurface(e, u, v + eps);
    const Position v2 = PointOnSurface(e, u, v - eps);

    return (v1.vec - v2.vec) / (2.f * eps);
}
