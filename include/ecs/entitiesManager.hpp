#pragma once

#include "idManager.hpp"


using Entity = Id;


class EntitiesManager {
public:
    inline Entity CreateEntity()
        { return idManager.CreateNewId(); }

    inline void DestroyEntity(Entity entity)
        { idManager.DestroyId(entity); }

private:
    IdManager idManager;

};
