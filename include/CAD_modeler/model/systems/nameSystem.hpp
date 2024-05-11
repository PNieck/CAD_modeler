#pragma once


#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>
#include "../components/name.hpp"


class NameSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator) {
        coordinator.RegisterSystem<NameSystem>();
        coordinator.RegisterRequiredComponent<NameSystem, Name>();
    }

    inline const std::unordered_set<Entity>& EntitiesWithNames() const
        { return entities; }

    inline const Name& GetName(Entity entity) const
        { return coordinator->GetComponent<Name>(entity); }

    inline void SetName(Entity entity, const Name& name) const
        { coordinator->SetComponent<Name>(entity, name); }
};
