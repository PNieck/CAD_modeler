#include <CAD_modeler/model/systems/nameSystem.hpp>


void NameSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterComponent<Name>();
    coordinator.RegisterSystem<NameSystem>();
    coordinator.RegisterRequiredComponent<NameSystem, Name>();
}


void NameSystem::SetName(Entity entity, const Name& name) const
{
    if (coordinator->HasComponent<Name>(entity))
        coordinator->SetComponent(entity, name);
    else
        coordinator->AddComponent(entity, name);
}
