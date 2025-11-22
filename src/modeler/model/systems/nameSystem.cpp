#include <CAD_modeler/model/systems/nameSystem.hpp>


void NameSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterComponent<Name>();
    coordinator.RegisterSystem<NameSystem>();
    coordinator.RegisterRequiredComponent<NameSystem, Name>();
}


void NameSystem::SetName(const Entity entity, const Name& name) const
{
    if (coordinator->HasComponent<Name>(entity))
        coordinator->SetComponent(entity, name);
    else
        coordinator->AddComponent(entity, name);
}


Entity NameSystem::EntityFromName(const Name &name) const
{
    // TODO: write something faster

    for (const Entity e : entities) {
        const auto& actName = coordinator->GetComponent<Name>(e);
        if (actName == name)
            return e;
    }

    throw std::runtime_error("Entity from name not found");
}
