#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include <set>
#include <optional>


class ToUpdateSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    template <typename Sys>
    inline void MarkAsToUpdate(Entity entity) {
        MarkAsToUpdate(entity, Coordinator::GetSystemID<Sys>());
    }

    void MarkAsToUpdate(Entity entity, SystemId systemId)
        { entitiesToUpdate[systemId].insert(entity); }

    template <typename Sys>
    void Unmark(Entity entity)
        { Umark(entity, Coordinator::GetSystemID<Sys>()); }

    void Unmark(Entity entity, SystemId systemId);

    template <typename Sys>
    void UnmarkAll()
        { UnmarkAll(Coordinator::GetSystemID<Sys>()); }

    void UnmarkAll(SystemId system)
        { entitiesToUpdate.erase(system); }

    template <typename Sys>
    const std::set<Entity>& GetEntitiesToUpdate() const
        { return GetEntitiesToUpdate(Coordinator::GetSystemID<Sys>()); }

    const std::set<Entity>& GetEntitiesToUpdate(SystemId systemId) const;

private:
    std::unordered_map<SystemId, std::set<Entity>> entitiesToUpdate;
    std::set<Entity> emptySet;
};
