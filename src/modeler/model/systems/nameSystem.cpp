#include <CAD_modeler/model/systems/nameSystem.hpp>

#include <ecs/coordinator.hpp>


void NameSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<NameSystem>();

    coordinator.RegisterRequiredComponent<NameSystem, Name>();
}


const Name& NameSystem::GetName(Entity entity) const
{
    return coordinator->GetComponent<Name>(entity);
}


void NameSystem::SetName(Entity entity, const Name& name) const
{
    coordinator->GetComponent<Name>(entity) = name;
}
