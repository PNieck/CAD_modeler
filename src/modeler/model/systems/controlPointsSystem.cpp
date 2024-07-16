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
    Entity object = coordinator->CreateEntity();

    ControlPoints controlPoints(entities);

    auto handler = std::make_shared<ControlPointMovedHandler>(object, *coordinator);

    for (Entity entity: entities) {
        auto handlerId = coordinator->Subscribe<Position>(entity, std::static_pointer_cast<EventHandler<Position>>(handler));
        controlPoints.controlPointsHandlers.insert({ entity, handlerId });
        RegisterControlPoint(entity);
    }

    controlPoints.deletionHandler = coordinator->Subscribe<ControlPoints>(object, std::static_pointer_cast<EventHandler<ControlPoints>>(deletionHandler));

     coordinator->AddComponent<ControlPoints>(object, controlPoints);

    return object;
}


void ControlPointsSystem::AddControlPoint(Entity object, Entity controlPoint)
{
    coordinator->EditComponent<ControlPoints>(object,
        [controlPoint, this](ControlPoints& params) {
            auto const& controlPoints = params.GetPoints();
            Entity prevControlPoint = *controlPoints.begin();
            HandlerId handlerId = params.controlPointsHandlers.at(prevControlPoint);
            auto eventHandler = coordinator->GetEventHandler<Position>(prevControlPoint, handlerId);

            params.AddControlPoint(controlPoint);
            params.controlPointsHandlers.insert({controlPoint, coordinator->Subscribe<Position>(controlPoint, eventHandler)});
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(object);

    RegisterControlPoint(controlPoint);
}


void ControlPointsSystem::DeleteControlPoint(Entity object, Entity controlPoint)
{
    coordinator->EditComponent<ControlPoints>(object,
        [object, controlPoint, this](ControlPoints& params) {
            params.DeleteControlPoint(controlPoint);
            coordinator->Unsubscribe<Position>(controlPoint, params.controlPointsHandlers.at(controlPoint));
            params.controlPointsHandlers.erase(controlPoint);
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(object);

    UnregisterControlPoint(controlPoint);
}


void ControlPointsSystem::RegisterControlPoint(Entity controlPoint)
{
    if (numberOfObjectsConnectedToControlPoint.contains(controlPoint))
        numberOfObjectsConnectedToControlPoint.at(controlPoint)++;
    else
        numberOfObjectsConnectedToControlPoint.insert( {controlPoint, 1u} );
}


void ControlPointsSystem::UnregisterControlPoint(Entity controlPoint)
{
    if (!numberOfObjectsConnectedToControlPoint.contains(controlPoint))
        return;

    unsigned int& cnt = numberOfObjectsConnectedToControlPoint.at(controlPoint);
    
    if (cnt == 1)
        numberOfObjectsConnectedToControlPoint.erase(controlPoint);
    else
        cnt--;
}


void ControlPointsSystem::ControlPointMovedHandler::HandleEvent(Entity entity, const Position & component, EventType eventType)
{
    if (eventType == EventType::ComponentDeleted) {
        coordinator.EditComponent<ControlPoints>(targetObject,
            [this, entity](ControlPoints& ctrlPts) {
                ctrlPts.DeleteControlPoint(entity);

                coordinator.Unsubscribe<Position>(entity, ctrlPts.controlPointsHandlers.at(entity));
                ctrlPts.controlPointsHandlers.erase(entity);
            }
        );
    }

    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate(targetObject);
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
