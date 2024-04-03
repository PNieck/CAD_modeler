#include <CAD_modeler/model/systems/nameSystem.hpp>

#include <CAD_modeler/model/ecsCoordinator.hpp>


void NameSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<NameSystem>();

    coordinator.RegisterComponent<Name>();

    coordinator.RegisterRequiredComponent<NameSystem, Name>();
}


Name & NameSystem::GetName(Entity entity) const
{
    return coordinator->GetComponent<Name>(entity);
}