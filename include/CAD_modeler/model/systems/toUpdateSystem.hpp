#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "../components/toUpdate.hpp"


class ToUpdateSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void MarkAsToUpdate(Entity entity) {
        coordinator->AddComponent<ToUpdate>(entity, ToUpdate());
    }

    inline void Unmark(Entity entity) {
        if (entities.contains(entity))
            coordinator->DeleteComponent<ToUpdate>(entity);
    }
};
