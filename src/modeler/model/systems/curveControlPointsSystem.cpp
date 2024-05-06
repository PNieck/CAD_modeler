#include <CAD_modeler/model/systems/curveControlPointsSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/curveControlPoints.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>


void CurveControlPointsSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<CurveControlPointsSystem>();

    coordinator.RegisterRequiredComponent<CurveControlPointsSystem, CurveControlPoints>();
}


void CurveControlPointsSystem::Init()
{
    deletionHandler = std::make_shared<DeletionHandler>(*coordinator);
}


Entity CurveControlPointsSystem::CreateControlPoints(const std::vector<Entity>& entities)
{
    Entity curve = coordinator->CreateEntity();

    CurveControlPoints controlPoints(entities);

    auto handler = std::make_shared<ControlPointMovedHandler>(curve, *coordinator);

    for (Entity entity: entities) {
        auto handlerId = coordinator->Subscribe<Position>(entity, std::static_pointer_cast<EventHandler<Position>>(handler));
        controlPoints.controlPointsHandlers.insert({ entity, handlerId });
    }

    controlPoints.deletionHandler = coordinator->Subscribe<CurveControlPoints>(curve, std::static_pointer_cast<EventHandler<CurveControlPoints>>(deletionHandler));

     coordinator->AddComponent<CurveControlPoints>(curve, controlPoints);

    return curve;
}


void CurveControlPointsSystem::AddControlPoint(Entity curve, Entity entity)
{
    coordinator->EditComponent<CurveControlPoints>(curve,
        [entity, this](CurveControlPoints& params) {
            auto const& controlPoints = params.ControlPoints();
            Entity controlPoint = *controlPoints.begin();
            HandlerId handlerId = params.controlPointsHandlers.at(controlPoint);
            auto eventHandler = coordinator->GetEventHandler<Position>(controlPoint, handlerId);

            params.AddControlPoint(entity);
            params.controlPointsHandlers.insert({entity, coordinator->Subscribe<Position>(entity, eventHandler)});
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(curve);
}


void CurveControlPointsSystem::DeleteControlPoint(Entity curve, Entity entity)
{
    bool entityDeleted = false;

    coordinator->EditComponent<CurveControlPoints>(curve,
        [&entityDeleted, curve, entity, this](CurveControlPoints& params) {
            params.DeleteControlPoint(entity);
            coordinator->Unsubscribe<Position>(entity, params.controlPointsHandlers.at(entity));
            params.controlPointsHandlers.erase(entity);

            if (params.controlPointsHandlers.size() == 0) {
                coordinator->DestroyEntity(curve);
                entityDeleted = true;
            }
        }
    );

    if (!entityDeleted)
        coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(curve);
}


void CurveControlPointsSystem::ControlPointMovedHandler::HandleEvent(Entity entity, const Position & component, EventType eventType)
{
    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate(curve);
}


void CurveControlPointsSystem::DeletionHandler::HandleEvent(Entity entity, const CurveControlPoints& component, EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    auto entitiesIt = component.ControlPoints().begin();
    auto handlersIt = component.controlPointsHandlers.begin();

    while (handlersIt != component.controlPointsHandlers.end() && entitiesIt != component.ControlPoints().end()) {
        coordinator.Unsubscribe<Position>(*entitiesIt, (*handlersIt).second);

        ++entitiesIt;
        ++handlersIt;
    }

    coordinator.Unsubscribe<CurveControlPoints>(entity, component.deletionHandler);
}
