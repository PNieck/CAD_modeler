#ifndef COMPONENTS_COLLECTION_H
#define COMPONENTS_COLLECTION_H


#include "../entitiesManager.hpp"

#include <unordered_map>

class IComponentCollection
{
public:
	virtual ~IComponentCollection() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
};


template<typename T>
class ComponentCollection: public IComponentCollection {
public:
    inline void AddComponent(Entity entity, T& component) {
        components.insert({ entity, component });
    }

    inline void DeleteComponent(Entity entity) {
        components.erase(entity);
    }

    inline T& GetComponent(Entity entity) {
        return components[entity];
    }

    void EntityDestroyed(Entity entity) override
	{
		components.erase(entity);
	}


private:
    std::unordered_map<Entity, T> components;
};


#endif
