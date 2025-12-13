#include <CAD_modeler/model/systems/intersectionSystem/equidistanceC0Surface.hpp>

#include <CAD_modeler/model/components/equidistantSurfaceParameters.hpp>


interSys::EquidistanceC0Surface::EquidistanceC0Surface(const Coordinator &coordinator, const Entity entity):
    C0Surface(coordinator, coordinator.GetComponent<EquidistanceSurfaceParameters>(entity).baseSurface),
    system(coordinator.GetSystem<EquidistanceC0SurfaceSystem>()),
    entity(entity)
{ }


alg::Vec3 interSys::EquidistanceC0Surface::PointOnSurface(float u, float v)
{
    Normalize(u, v);

    return system->PointOnSurface(entity, u, v).vec;
}


alg::Vec3 interSys::EquidistanceC0Surface::PartialDerivativeU(float u, float v)
{
    Normalize(u, v);

    return system->PartialDerivativeU(entity, u, v);
}


alg::Vec3 interSys::EquidistanceC0Surface::PartialDerivativeV(float u, float v)
{
    Normalize(u, v);

    return system->PartialDerivativeV(entity, u, v);
}
