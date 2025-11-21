#include <CAD_modeler/model/systems/intersectionSystem/equidistanceSurface.hpp>

#include <CAD_modeler/model/components/equidistantSurfaceParameters.hpp>


interSys::EquidistanceSystem::EquidistanceSystem(const Coordinator &coordinator, const Entity entity):
    C2Surface(coordinator, coordinator.GetComponent<EquidistanceSurfaceParameters>(entity).baseSurface),
    system(coordinator.GetSystem<EquidistanceC2System>()),
    entity(entity)
{ }


alg::Vec3 interSys::EquidistanceSystem::PointOnSurface(float u, float v)
{
    Normalize(u, v);

    return system->PointOnSurface(entity, u, v).vec;
}


alg::Vec3 interSys::EquidistanceSystem::PartialDerivativeU(float u, float v)
{
    Normalize(u, v);

    return system->PartialDerivativeU(entity, u, v);
}


alg::Vec3 interSys::EquidistanceSystem::PartialDerivativeV(float u, float v)
{
    Normalize(u, v);

    return system->PartialDerivativeV(entity, u, v);
}
