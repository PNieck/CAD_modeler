#include <CAD_modeler/model/systems/controlPointsSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/controlPoints.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>


void ControlPointsSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<ControlPointsSystem>();

    coordinator.RegisterRequiredComponent<ControlPointsSystem, ControlPoints>();
}


void ControlPointsSystem::Init()
{
    deletionHandler = std::make_shared<DeletionHandler>(*coordinator);
}


Entity ControlPointsSystem::CreateControlPoints(const std::vector<Entity>& entities)
{
    Entity curve = coordinator->CreateEntity();

    ControlPoints controlPoints(entities);

    auto handler = std::make_shared<ControlPointMovedHandler>(curve, *coordinator);

    for (Entity entity: entities) {
        auto handlerId = coordinator->Subscribe<Position>(entity, std::static_pointer_cast<EventHandler<Position>>(handler));
        controlPoints.controlPointsHandlers.insert({ entity, handlerId });
    }

    controlPoints.deletionHandler = coordinator->Subscribe<ControlPoints>(curve, std::static_pointer_cast<EventHandler<ControlPoints>>(deletionHandler));

     coordinator->AddComponent<ControlPoints>(curve, controlPoints);

    return curve;
}


void ControlPointsSystem::AddControlPoint(Entity curve, Entity entity)
{
    coordinator->EditComponent<ControlPoints>(curve,
        [entity, this](ControlPoints& params) {
            auto const& controlPoints = params.GetPoints();
            Entity controlPoint = *controlPoints.begin();
            HandlerId handlerId = params.controlPointsHandlers.at(controlPoint);
            auto eventHandler = coordinator->GetEventHandler<Position>(controlPoint, handlerId);

            params.AddControlPoint(entity);
            params.controlPointsHandlers.insert({entity, coordinator->Subscribe<Position>(entity, eventHandler)});
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(curve);
}


void ControlPointsSystem::DeleteControlPoint(Entity curve, Entity entity)
{
    bool entityDeleted = false;

    coordinator->EditComponent<ControlPoints>(curve,
        [&entityDeleted, curve, entity, this](ControlPoints& params) {
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


void ControlPointsSystem::ControlPointMovedHandler::HandleEvent(Entity entity, const Position & component, EventType eventType)
{
    if (eventType == EventType::ComponentDeleted) {
        coordinator.EditComponent<ControlPoints>(curve,
            [this, entity](ControlPoints& ctrlPts) {
                ctrlPts.DeleteControlPoint(entity);

                coordinator.Unsubscribe<Position>(entity, ctrlPts.controlPointsHandlers.at(entity));
                ctrlPts.controlPointsHandlers.erase(entity);
            }
        );
    }

    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate(curve);
}


void ControlPointsSystem::DeletionHandler::HandleEvent(Entity entity, const ControlPoints& component, EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    auto entitiesIt = component.GetPoints().begin();
    auto handlersIt = component.controlPointsHandlers.begin();

    while (handlersIt != component.controlPointsHandlers.end() && entitiesIt != component.GetPoints().end()) {
        coordinator.Unsubscribe<Position>(*entitiesIt, (*handlersIt).second);

        ++entitiesIt;
        ++handlersIt;
    }

    coordinator.Unsubscribe<ControlPoints>(entity, component.deletionHandler);
}
