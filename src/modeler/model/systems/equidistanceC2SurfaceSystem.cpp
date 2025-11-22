#include <CAD_modeler/model/systems/equidistanceC2SurfaceSystem.hpp>

#include <CAD_modeler/model/components/equidistantSurfaceParameters.hpp>
#include <CAD_modeler/model/components/wraps.hpp>


void EquidistanceC2System::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<EquidistanceC2System>();

    coordinator.RegisterComponent<EquidistanceSurfaceParameters>();
}


void EquidistanceC2System::Init()
{
    c2PatchesSystem = coordinator->GetSystem<C2PatchesSystem>();
}


Entity EquidistanceC2System::AddSurface(const Entity base, const float dist)
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


Position EquidistanceC2System::PointOnSurface(const Entity e, const float u, const float v) const
{
    const auto [baseSurface, distance] = coordinator->GetComponent<EquidistanceSurfaceParameters>(e);

    const Position basePos = c2PatchesSystem->PointOnSurface(baseSurface, u, v);
    const alg::Vec3 baseNormal = c2PatchesSystem->NormalVector(baseSurface, u, v);

    return basePos.vec + distance * baseNormal.Normalize();
}


alg::Vec3 EquidistanceC2System::PartialDerivativeU(Entity e, float u, float v) const
{
    const auto [baseSurface, distance] = coordinator->GetComponent<EquidistanceSurfaceParameters>(e);
    auto const& c2Patches = coordinator->GetComponent<C2Patches>(baseSurface);

    alg::Vec3 partialU = c2PatchesSystem->PartialDerivativeU(c2Patches, u, v);
    alg::Vec3 partialV = c2PatchesSystem->PartialDerivativeV(c2Patches, u, v);

    // alg::Vec3 partialUU = c2PatchesSystem->PartialDerivativeUUApprox(c2Patches, u, v);
    // alg::Vec3 partialUV = c2PatchesSystem->PartialDerivativeUVApprox(c2Patches, u, v);

    alg::Vec3 partialUU = c2PatchesSystem->PartialDerivativeUU(c2Patches, u, v);
    alg::Vec3 partialUV = c2PatchesSystem->PartialDerivativeUV(c2Patches, u, v);

    // std::cout << "UU diff: " << (partialUU - partialUUApprox).Length() / partialUUApprox.Length() << "\n";
    // std::cout << "UV diff: " << (partialUV - partialUVApprox).Length() / partialUVApprox.Length() << "\n";

    alg::Vec3 s = Cross(partialU, partialV);
    float sLenSq = s.LengthSquared();
    float sLen = sqrt(sLenSq);

    alg::Vec3 partialSU = Cross(partialUU, partialV) + Cross(partialU, partialUV);

    float partialSLenU = Dot(s, partialSU) / sLen;

    alg::Vec3 partialNU = (sLen * partialSU - partialSLenU * s) / sLenSq;

    return partialU + distance * partialNU;

    /*
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
    */
}


alg::Vec3 EquidistanceC2System::PartialDerivativeV(Entity e, float u, float v) const
{
    const auto [baseSurface, distance] = coordinator->GetComponent<EquidistanceSurfaceParameters>(e);
    auto const& c2Patches = coordinator->GetComponent<C2Patches>(baseSurface);

    alg::Vec3 partialU = c2PatchesSystem->PartialDerivativeU(c2Patches, u, v);
    alg::Vec3 partialV = c2PatchesSystem->PartialDerivativeV(c2Patches, u, v);

    // alg::Vec3 partialVV = c2PatchesSystem->PartialDerivativeVVApprox(c2Patches, u, v);
    // alg::Vec3 partialUV = c2PatchesSystem->PartialDerivativeUVApprox(c2Patches, u, v);

    alg::Vec3 partialVV = c2PatchesSystem->PartialDerivativeVV(c2Patches, u, v);
    alg::Vec3 partialUV = c2PatchesSystem->PartialDerivativeUV(c2Patches, u, v);
    //
    // std::cout << "VV diff: " << (partialVV - partialVVApprox).Length() / partialVVApprox.Length() << "\n";
    // std::cout << "UV diff: " << (partialUV - partialUVApprox).Length() / partialUVApprox.Length() << "\n";

    alg::Vec3 s = Cross(partialU, partialV);
    float sLenSq = s.LengthSquared();
    float sLen = sqrt(sLenSq);

    alg::Vec3 partialSV = Cross(partialUV, partialV) + Cross(partialU, partialVV);

    float partialSLenV = Dot(s, partialSV) / sLen;

    alg::Vec3 partialNV = (sLen * partialSV - partialSLenV * s) / sLenSq;

    return partialV + distance * partialNV;

    /*
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
    */
}


alg::Vec3 EquidistanceC2System::NormalVector(const Entity e, const float u, const float v) const
{
    const Entity baseSurface = coordinator->GetComponent<EquidistanceSurfaceParameters>(e).baseSurface;
    return c2PatchesSystem->NormalVector(baseSurface, u, v);
}


float EquidistanceC2System::MaxU(const Entity e) const
{
    const Entity baseSurface = coordinator->GetComponent<EquidistanceSurfaceParameters>(e).baseSurface;
    return c2PatchesSystem->MaxU(baseSurface);
}


float EquidistanceC2System::MaxV(const Entity e) const
{
    const Entity baseSurface = coordinator->GetComponent<EquidistanceSurfaceParameters>(e).baseSurface;
    return c2PatchesSystem->MaxV(baseSurface);
}
