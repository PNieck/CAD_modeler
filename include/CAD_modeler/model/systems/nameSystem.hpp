#pragma once


#include <ecs/system.hpp>
#include "../components/name.hpp"


class NameSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline const std::unordered_set<Entity>& EntitiesWithNames() const
        { return entities; }

    Name& GetName(Entity entity) const;
};
