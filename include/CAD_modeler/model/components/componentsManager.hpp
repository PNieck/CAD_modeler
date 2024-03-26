#ifndef COMPONENTS_MANAGER_H
#define COMPONENTS_MANAGER_H

#include "../entitiesManager.hpp"
#include "componentsCollection.hpp"

#include <unordered_map>
#include <memory>
#include <set>


using ComponentId = std::size_t;


class ComponentsManager {
public:
    template<typename T>
    void RegisterComponent() {
        std::size_t hash = GetComponentId<T>();
        components.insert({hash, std::make_shared<ComponentCollection<T>>()});
    }

    template<typename T>
    inline void AddComponent(Entity entity, T& component) {
        GetComponentCollection<T>()->AddComponent(entity, component);
        componentsOfEntities[entity].insert(GetComponentId<T>());
    }

    template<typename T>
    inline void DeleteComponent(Entity entity) {
        GetComponentCollection<T>()->DeleteComponent(entity);
        componentsOfEntities[entity].erase(GetComponentId<T>());
    }

    template<typename T>
    inline T& GetComponent(Entity entity) {
        return GetComponentCollection<T>()->GetComponent(entity);
    }

    inline const std::set<ComponentId>& GetEntityComponents(Entity entity) {
        return componentsOfEntities[entity];
    }

    void EntityDeleted(Entity entity) {
        componentsOfEntities.erase(entity);
        for (auto const& pair: components) {
            pair.second->EntityDestroyed(entity);
        }
    }

    template <typename T>
    static constexpr ComponentId GetComponentId() {
        return typeid(T).hash_code();
    }

private:
    std::unordered_map<ComponentId, std::shared_ptr<IComponentCollection>> components;

    std::unordered_map<Entity, std::set<ComponentId>> componentsOfEntities;

    template<typename T>
    std::shared_ptr<ComponentCollection<T>> GetComponentCollection() {
        std::size_t hash = GetComponentId<T>();
        return std::static_pointer_cast<ComponentCollection<T>>(components[hash]);
    }
};


#endif
