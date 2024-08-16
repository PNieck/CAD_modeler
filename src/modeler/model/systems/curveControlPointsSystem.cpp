#include <CAD_modeler/model/systems/curveControlPointsSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/curveControlPoints.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>
#include <CAD_modeler/model/systems/controlPointsRegistrySystem.hpp>


void CurveControlPointsSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<CurveControlPointsSystem>();

    coordinator.RegisterRequiredComponent<CurveControlPointsSystem, CurveControlPoints>();
}


void CurveControlPointsSystem::Init()
{
    deletionHandler = std::make_shared<DeletionHandler>(*coordinator);
}


Entity CurveControlPointsSystem::CreateControlPoints(const std::vector<Entity>& entities, SystemId system)
{
    Entity object = coordinator->CreateEntity();
    CurveControlPoints controlPoints(entities);

    auto handler = std::make_shared<ControlPointMovedHandler>(object, *coordinator);
    auto registry = coordinator->GetSystem<ControlPointsRegistrySystem>();

    for (Entity entity: entities) {
        auto handlerId = coordinator->Subscribe<Position>(entity, std::static_pointer_cast<EventHandler<Position>>(handler));
        controlPoints.controlPointsHandlers.insert({ entity, handlerId });
        registry->RegisterControlPoint(object, entity, system);
    }

    controlPoints.deletionHandler = coordinator->Subscribe<CurveControlPoints>(object, std::static_pointer_cast<EventHandler<CurveControlPoints>>(deletionHandler));

     coordinator->AddComponent<CurveControlPoints>(object, controlPoints);

    return object;
}


void CurveControlPointsSystem::AddControlPoint(Entity object, Entity controlPoint, SystemId system)
{
    coordinator->EditComponent<CurveControlPoints>(object,
        [controlPoint, this](CurveControlPoints& params) {
            auto const& controlPoints = params.GetPoints();
            Entity prevControlPoint = *controlPoints.begin();
            HandlerId handlerId = params.controlPointsHandlers.at(prevControlPoint);
            auto eventHandler = coordinator->GetEventHandler<Position>(prevControlPoint, handlerId);

            params.AddControlPoint(controlPoint);
            params.controlPointsHandlers.insert({controlPoint, coordinator->Subscribe<Position>(controlPoint, eventHandler)});
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(object);

    auto registry = coordinator->GetSystem<ControlPointsRegistrySystem>();
    registry->RegisterControlPoint(object, controlPoint, system);
}


void CurveControlPointsSystem::DeleteControlPoint(Entity object, Entity controlPoint, SystemId system)
{
    coordinator->EditComponent<CurveControlPoints>(object,
        [object, controlPoint, this](CurveControlPoints& params) {
            params.DeleteControlPoint(controlPoint);
            coordinator->Unsubscribe<Position>(controlPoint, params.controlPointsHandlers.at(controlPoint));
            params.controlPointsHandlers.erase(controlPoint);
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(object);

    auto registry = coordinator->GetSystem<ControlPointsRegistrySystem>();
    registry->UnregisterControlPoint(object, controlPoint, system);
}


void CurveControlPointsSystem::ControlPointMovedHandler::HandleEvent(Entity entity, const Position & component, EventType eventType)
{
    if (eventType == EventType::ComponentDeleted) {
        coordinator.EditComponent<CurveControlPoints>(targetObject,
            [this, entity](CurveControlPoints& ctrlPts) {
                ctrlPts.DeleteControlPoint(entity);

                coordinator.Unsubscribe<Position>(entity, ctrlPts.controlPointsHandlers.at(entity));
                ctrlPts.controlPointsHandlers.erase(entity);
            }
        );
    }

    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate(targetObject);
}


void CurveControlPointsSystem::DeletionHandler::HandleEvent(Entity entity, const CurveControlPoints& component, EventType eventType)
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

    coordinator.Unsubscribe<CurveControlPoints>(entity, component.deletionHandler);
}
