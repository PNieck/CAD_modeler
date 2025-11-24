#include <CAD_modeler/model/systems/intersectionSystem/equidistanceC2Surface.hpp>

#include <CAD_modeler/model/components/equidistantSurfaceParameters.hpp>


interSys::EquidistanceC2Surface::EquidistanceC2Surface(const Coordinator &coordinator, const Entity entity):
    C2Surface(coordinator, coordinator.GetComponent<EquidistanceSurfaceParameters>(entity).baseSurface),
    system(coordinator.GetSystem<EquidistanceC2SurfaceSystem>()),
    entity(entity)
{ }


alg::Vec3 interSys::EquidistanceC2Surface::PointOnSurface(float u, float v)
{
    Normalize(u, v);

    return system->PointOnSurface(entity, u, v).vec;
}


alg::Vec3 interSys::EquidistanceC2Surface::PartialDerivativeU(float u, float v)
{
    Normalize(u, v);

    return system->PartialDerivativeU(entity, u, v);
}


alg::Vec3 interSys::EquidistanceC2Surface::PartialDerivativeV(float u, float v)
{
    Normalize(u, v);

    return system->PartialDerivativeV(entity, u, v);
}
