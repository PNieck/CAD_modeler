#ifndef SYSTEM_H
#define SYSTEM_H

#include "../entitiesManager.hpp"

#include <unordered_set>
#include <memory.h>


class Coordinator;


class System {
public:

    inline void RemoveEntity(Entity entity) {
        entities.erase(entity);
    }

    inline void AddEntity(Entity entity) {
        entities.insert(entity);
    }

    inline void SetCoordinator(Coordinator* coordinator) {
        this->coordinator = coordinator;
    }

protected:
    std::unordered_set<Entity> entities;
    Coordinator* coordinator;
};


#endif
