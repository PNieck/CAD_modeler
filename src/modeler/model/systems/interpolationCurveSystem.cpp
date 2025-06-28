#include <CAD_modeler/model/systems/interpolationCurveSystem.hpp>

#include <CAD_modeler/model/systems/interpolationCurvesRenderingSystem.hpp>


void InterpolationCurveSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<InterpolationCurveSystem>();
    coordinator.RegisterSystem<InterpolationCurvesRenderingSystem>();
    coordinator.RegisterSystem<CurveControlPointsSystem>();
}


Entity InterpolationCurveSystem::CreateCurve(const std::vector<Entity>& controlPoints)
{
    const Entity curve = coordinator->GetSystem<InterpolationCurvesRenderingSystem>()->AddCurve(controlPoints);

    entities.insert(curve);

    return curve;
}


void InterpolationCurveSystem::AddControlPoint(const Entity curve, const Entity cp) const {
    const auto sys = coordinator->GetSystem<CurveControlPointsSystem>();
    const SystemId sysId = Coordinator::GetSystemID<InterpolationCurvesRenderingSystem>();

    sys->AddControlPoint(curve, cp, sysId);
}


void InterpolationCurveSystem::DeleteControlPoint(const Entity curve, const Entity cp) const {
    const auto sys = coordinator->GetSystem<CurveControlPointsSystem>();
    const SystemId sysId = Coordinator::GetSystemID<InterpolationCurvesRenderingSystem>();

    sys->DeleteControlPoint(curve, cp, sysId);
}


void InterpolationCurveSystem::MergeControlPoints(const Entity curve, const Entity oldCP, const Entity newCP) const {
    const auto sys = coordinator->GetSystem<CurveControlPointsSystem>();
    const SystemId sysId = Coordinator::GetSystemID<InterpolationCurvesRenderingSystem>();

    sys->MergeControlPoints(curve, oldCP, newCP, sysId);
}
