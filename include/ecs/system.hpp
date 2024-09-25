#pragma once

#include "entitiesManager.hpp"

#include <unordered_set>


class Coordinator;


class System {
public:

    inline void RemoveEntity(Entity entity) {
        entities.erase(entity);
    }

    inline void AddEntity(Entity entity) {
        entities.insert(entity);
    }

    // TODO: remove
    inline void SetCoordinator(Coordinator* coord) {
        this->coordinator = coord;
    }

    inline const std::unordered_set<Entity>& GetEntities() const {
        return entities;
    }

protected:
    std::unordered_set<Entity> entities;
    Coordinator* coordinator;
};
