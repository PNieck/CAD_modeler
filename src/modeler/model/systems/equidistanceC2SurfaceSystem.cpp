#include <CAD_modeler/model/systems/equidistanceC2SurfaceSystem.hpp>

#include <CAD_modeler/model/components/equidistantSurfaceParameters.hpp>

#include <CAD_modeler/model/systems/c2PatchesSystem.hpp>

#include <ecs/coordinator.hpp>


void EquidistanceC2SurfaceSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<EquidistanceC2SurfaceSystem>();

    coordinator.RegisterComponent<EquidistanceSurfaceParameters>();
}


void EquidistanceC2SurfaceSystem::Init()
{
    baseSystem = coordinator->GetSystem<C2PatchesSystem>();
}


alg::Vec3 EquidistanceC2SurfaceSystem::PartialDerivativeU(const Entity e, const float u, const float v) const
{
    const auto [baseSurface, distance] = coordinator->GetComponent<EquidistanceSurfaceParameters>(e);
    auto const& c2Patches = coordinator->GetComponent<C2Patches>(baseSurface);

    auto const& c2PatchesSystem = coordinator->GetSystem<C2PatchesSystem>();

    const alg::Vec3 partialU = c2PatchesSystem->PartialDerivativeU(c2Patches, u, v);
    const alg::Vec3 partialV = c2PatchesSystem->PartialDerivativeV(c2Patches, u, v);

    // alg::Vec3 partialUU = c2PatchesSystem->PartialDerivativeUUApprox(c2Patches, u, v);
    // alg::Vec3 partialUV = c2PatchesSystem->PartialDerivativeUVApprox(c2Patches, u, v);

    const alg::Vec3 partialUU = c2PatchesSystem->PartialDerivativeUU(c2Patches, u, v);
    const alg::Vec3 partialUV = c2PatchesSystem->PartialDerivativeUV(c2Patches, u, v);

    // std::cout << "UU diff: " << (partialUU - partialUUApprox).Length() / partialUUApprox.Length() << "\n";
    // std::cout << "UV diff: " << (partialUV - partialUVApprox).Length() / partialUVApprox.Length() << "\n";

    return EquidistanceSurfaceSystem::PartialDerivativeU(distance, partialU, partialV, partialUU, partialUV);
}


alg::Vec3 EquidistanceC2SurfaceSystem::PartialDerivativeV(Entity e, float u, float v) const
{
    const auto [baseSurface, distance] = coordinator->GetComponent<EquidistanceSurfaceParameters>(e);
    auto const& c2Patches = coordinator->GetComponent<C2Patches>(baseSurface);

    auto const& c2PatchesSystem = coordinator->GetSystem<C2PatchesSystem>();

    alg::Vec3 partialU = c2PatchesSystem->PartialDerivativeU(c2Patches, u, v);
    alg::Vec3 partialV = c2PatchesSystem->PartialDerivativeV(c2Patches, u, v);

    // alg::Vec3 partialVV = c2PatchesSystem->PartialDerivativeVVApprox(c2Patches, u, v);
    // alg::Vec3 partialUV = c2PatchesSystem->PartialDerivativeUVApprox(c2Patches, u, v);

    alg::Vec3 partialVV = c2PatchesSystem->PartialDerivativeVV(c2Patches, u, v);
    alg::Vec3 partialUV = c2PatchesSystem->PartialDerivativeUV(c2Patches, u, v);
    //
    // std::cout << "VV diff: " << (partialVV - partialVVApprox).Length() / partialVVApprox.Length() << "\n";
    // std::cout << "UV diff: " << (partialUV - partialUVApprox).Length() / partialUVApprox.Length() << "\n";

    return EquidistanceSurfaceSystem::PartialDerivativeV(distance, partialU, partialV, partialVV, partialUV);
}
