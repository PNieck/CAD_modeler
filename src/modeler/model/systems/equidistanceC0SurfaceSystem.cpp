#include <CAD_modeler/model/systems/equidistanceC0SurfaceSystem.hpp>

#include <CAD_modeler/model/components/equidistantSurfaceParameters.hpp>

#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>

#include <ecs/coordinator.hpp>


void EquidistanceC0SurfaceSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<EquidistanceC0SurfaceSystem>();

    coordinator.RegisterComponent<EquidistanceSurfaceParameters>();
}


void EquidistanceC0SurfaceSystem::Init()
{
    baseSystem = coordinator->GetSystem<C0PatchesSystem>();
}

alg::Vec3 EquidistanceC0SurfaceSystem::PartialDerivativeU(Entity e, float u, float v) const
{
    const auto [baseSurface, distance] = coordinator->GetComponent<EquidistanceSurfaceParameters>(e);
    auto const& c0Patches = coordinator->GetComponent<C0Patches>(baseSurface);

    auto const& c0PatchesSystem = coordinator->GetSystem<C0PatchesSystem>();

    const alg::Vec3 partialU = c0PatchesSystem->PartialDerivativeU(c0Patches, u, v);
    const alg::Vec3 partialV = c0PatchesSystem->PartialDerivativeV(c0Patches, u, v);

    const alg::Vec3 partialUU = baseSystem->PartialDerivativeUUApprox(baseSurface, u, v);
    const alg::Vec3 partialUV = baseSystem->PartialDerivativeUVApprox(baseSurface, u, v);

    return EquidistanceSurfaceSystem::PartialDerivativeU(distance, partialU, partialV, partialUU, partialUV);
}


alg::Vec3 EquidistanceC0SurfaceSystem::PartialDerivativeV(Entity e, float u, float v) const
{
    const auto [baseSurface, distance] = coordinator->GetComponent<EquidistanceSurfaceParameters>(e);
    auto const& c0Patches = coordinator->GetComponent<C0Patches>(baseSurface);

    auto const& c0PatchesSystem = coordinator->GetSystem<C0PatchesSystem>();

    const alg::Vec3 partialU = c0PatchesSystem->PartialDerivativeU(c0Patches, u, v);
    const alg::Vec3 partialV = c0PatchesSystem->PartialDerivativeV(c0Patches, u, v);

    const alg::Vec3 partialVV = baseSystem->PartialDerivativeVVApprox(baseSurface, u, v);
    const alg::Vec3 partialUV = baseSystem->PartialDerivativeUVApprox(baseSurface, u, v);

    return EquidistanceSurfaceSystem::PartialDerivativeV(distance, partialU, partialV, partialVV, partialUV);
}
