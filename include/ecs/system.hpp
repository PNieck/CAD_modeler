#pragma once

#include "entitiesManager.hpp"

#include <unordered_set>


class Coordinator;


class System {
public:
    virtual ~System() = default;

    System() = default;

    void RemoveEntity(const Entity entity) {
        entities.erase(entity);
    }

    void AddEntity(const Entity entity) {
        entities.insert(entity);
    }

    // TODO: remove
    void SetCoordinator(Coordinator* coord) {
        this->coordinator = coord;
    }

    const std::unordered_set<Entity>& GetEntities() const {
        return entities;
    }

protected:
    std::unordered_set<Entity> entities;
    Coordinator* coordinator;
};
