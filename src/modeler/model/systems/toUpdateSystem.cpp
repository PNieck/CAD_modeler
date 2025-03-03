#include <CAD_modeler/model/systems/toUpdateSystem.hpp>

#include <ecs/coordinator.hpp>


void ToUpdateSystem::RegisterSystem(Coordinator& coordinator)
{
    coordinator.RegisterSystem<ToUpdateSystem>();
}


void ToUpdateSystem::Unmark(Entity entity, SystemId systemId)
{
    auto it = entitiesToUpdate.find(systemId);
    if (it == entitiesToUpdate.end())
        return;

    it->second.erase(entity);

    if (it->second.empty())
        entitiesToUpdate.erase(systemId);
}


const std::set<Entity> &ToUpdateSystem::GetEntitiesToUpdate(SystemId systemId) const
{
    auto it = entitiesToUpdate.find(systemId);
    if (it == entitiesToUpdate.end())
        return emptySet;

    return it->second;
}
