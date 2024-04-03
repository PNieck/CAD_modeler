#pragma once


#include "system.hpp"


class SelectionSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline bool IsSelected(Entity entity) const
        { return entities.contains(entity); }

    void Select(Entity entity);

    void Deselect(Entity entity);

    inline const std::unordered_set<Entity>& SelectedEntities() const
        { return entities; }
};
