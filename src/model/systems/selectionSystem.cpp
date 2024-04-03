#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <CAD_modeler/model/ecsCoordinator.hpp>

#include <CAD_modeler/model/components/selected.hpp>

#include <stdexcept>


void SelectionSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<SelectionSystem>();

    coordinator.RegisterComponent<Selected>();

    coordinator.RegisterRequiredComponent<SelectionSystem, Selected>();
}


void SelectionSystem::Select(Entity entity)
{
    coordinator->AddComponent<Selected>(entity, Selected());
}


void SelectionSystem::Deselect(Entity entity)
{
    coordinator->DeleteComponent<Selected>(entity);
}
