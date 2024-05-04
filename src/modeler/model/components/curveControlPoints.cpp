#include <CAD_modeler/model/components/curveControlPoints.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>


void ControlPointMovedHandler::HandleEvent(Entity entity, const Position & component, EventType eventType)
{
    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate(curve);
}


void DeletionHandler::HandleEvent(Entity entity, const CurveControlPoints& component, EventType eventType)
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
