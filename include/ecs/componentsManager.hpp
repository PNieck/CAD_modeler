#pragma once

#include "entitiesManager.hpp"
#include "componentsCollection.hpp"

#include <unordered_map>
#include <memory>
#include <ranges>
#include <set>


using ComponentId = std::size_t;


class ComponentsManager {
public:
    template<typename T>
    void RegisterComponent() {
        std::size_t hash = GetComponentId<T>();
        components.insert({hash, std::make_shared<ComponentCollection<T>>()});
    }

    void RegisterNewEntity(Entity entity) {
        componentsOfEntities.insert({entity, {}});
    }

    template<typename T>
    void AddComponent(Entity entity, const T& component) {
        GetComponentCollection<T>()->AddComponent(entity, component);
        componentsOfEntities[entity].insert(GetComponentId<T>());
    }

    template<typename T>
    void AddComponent(Entity entity, T&& component) {
        GetComponentCollection<T>()->AddComponent(entity, component);
        componentsOfEntities[entity].insert(GetComponentId<T>());
    }

    template<typename T>
    void DeleteComponent(Entity entity) {
        GetComponentCollection<T>()->DeleteComponent(entity);
        componentsOfEntities[entity].erase(GetComponentId<T>());
    }

    template<typename T>
    T& GetComponent(Entity entity) const {
        return GetComponentCollection<T>()->GetComponent(entity);
    }

    [[nodiscard]]
    const std::set<ComponentId>& GetEntityComponents(const Entity entity) const {
        return componentsOfEntities.at(entity);
    }

    void EntityDeleted(const Entity entity) {
        for (const ComponentId componentId: componentsOfEntities.at(entity)) {
            const auto collection = components.at(componentId);
            collection->EntityDestroyed(entity);
        }

        componentsOfEntities.erase(entity);
    }

    template <typename T>
    static constexpr ComponentId GetComponentId() {
        return typeid(T).hash_code();
    }

    

private:
    std::unordered_map<ComponentId, std::shared_ptr<IComponentCollection>> components;

    std::unordered_map<Entity, std::set<ComponentId>> componentsOfEntities;

    template<typename T>
    std::shared_ptr<ComponentCollection<T>> GetComponentCollection() const {
        const ComponentId id = GetComponentId<T>();
        return std::static_pointer_cast<ComponentCollection<T>>(components.at(id));
    }
};
