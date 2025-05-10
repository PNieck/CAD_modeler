#pragma once

#include "entitiesManager.hpp"
#include "componentsCollection.hpp"
#include "emptyComponentConcept.hpp"

#include <unordered_map>
#include <memory>
#include <type_traits>
#include <set>


using ComponentId = std::size_t;


class ComponentsManager {
public:
    template<typename T>
    void RegisterComponent() {
        if constexpr (std::is_empty_v<T> == false) {
            std::size_t hash = GetComponentId<T>();
            components.insert({hash, std::make_shared<ComponentCollection<T>>()});
        }
    }

    void RegisterNewEntity(Entity entity) {
        componentsOfEntities.insert({entity, {}});
    }

    template<typename T>
    void AddComponent(Entity entity, const T& component) {
        componentsOfEntities[entity].insert(GetComponentId<T>());

        if constexpr (std::is_empty_v<T> == false)
            GetComponentCollection<T>()->AddComponent(entity, component);
    }

    // template<typename T>
    // void AddComponent(Entity entity, T&& component) {
    //     componentsOfEntities[entity].insert(GetComponentId<T>());
    //
    //     if constexpr (!std::is_empty_v<T>)
    //         GetComponentCollection<T>()->AddComponent(entity, component);
    // }

    template<typename T>
    void DeleteComponent(Entity entity) {
        componentsOfEntities[entity].erase(GetComponentId<T>());

        if constexpr (std::is_empty_v<T> == false)
            GetComponentCollection<T>()->DeleteComponent(entity);
    }

    template<NotEmptyComponent T>
    T& GetComponent(Entity entity) const {
        static_assert(std::is_empty_v<T> == false);

        return GetComponentCollection<T>()->GetComponent(entity);
    }

    template<EmptyComponent T>
    T GetComponent(const Entity entity) const {
        if (!componentsOfEntities.at(entity).contains(GetComponentId<T>()))
            throw std::out_of_range("Component does not exist");

        return T();
    }

    [[nodiscard]]
    const std::set<ComponentId>& GetEntityComponents(const Entity entity) const {
        return componentsOfEntities.at(entity);
    }

    void EntityDeleted(const Entity entity) {
        if constexpr (std::is_empty_v<ComponentId> == false) {
            for (const ComponentId componentId: componentsOfEntities.at(entity)) {
                const auto collection = components.at(componentId);
                collection->EntityDestroyed(entity);
            }
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
