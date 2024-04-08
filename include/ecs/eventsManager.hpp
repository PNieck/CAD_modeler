#pragma once

#include "entitiesManager.hpp"
#include "componentsManager.hpp"

#include <any>
#include <functional>
#include <unordered_map>
#include <unordered_set>


class EventsManager {
public:
    template <typename Comp>
    void SubscribeComponentChange(Entity entity, const std::function<void(Entity, const Comp&)>& function) {
        auto& interestedListeners = listeners[CreateKey<Comp>(entity)];

        if (interestedListeners.empty()) {
            observedComponents[entity].insert(ComponentsManager::GetComponentId<Comp>());
        }
        
        interestedListeners.insert(function);
    }

    template <typename Comp>
    void UnsubscribeComponentChange(Entity entity, const std::function<void(Entity, const Comp&)>& function) {
        auto& interestedListeners = listeners[CreateKey<Comp>(entity)];
        interestedListeners.erase(function);

        if (interestedListeners.empty()) {
            observedComponents.erase(entity);
        }
    }

    template <typename Comp>
    void ComponentChanged(Entity entity, const Comp& component) {
        auto const& interestedListeners = listeners[CreateKey<Comp>(entity)];

        for (auto const& listener : interestedListeners) {
            auto const& func = any_cast<std::function<void(Entity, const Comp&)>>(listener);
            func(entity, component);
        }
    }

    void EntityDeleted(Entity entity) {
        auto& components = observedComponents[entity];

        for (auto component: components) {
            EntityComponentKey key{ .entity=entity, .compId=component };
            listeners.erase(key);
        }
    }

private:
    struct EntityComponentKey {
        Entity entity;
        ComponentId compId;

        bool operator== (const EntityComponentKey& rhs) const {
            return this->entity == rhs.entity && this->compId == rhs.compId;
        }
    };


    struct EntityComponentKeyHash {
        size_t operator()(const EntityComponentKey& p) const
        {
            return hash_combine(std::hash<Entity>{}(p.entity), std::hash<ComponentId>{}(p.compId));
        }


        // Source: https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes
        static size_t hash_combine( size_t lhs, size_t rhs ) {
            if constexpr (sizeof(size_t) >= 8) {
                lhs ^= rhs + 0x517cc1b727220a95 + (lhs << 6) + (lhs >> 2);
            } else {
                lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
            }

            return lhs;
        }
    };

    struct AnyHash {
        size_t operator()(const std::any& val) {
            return val.type().hash_code();
        }
    };

    std::unordered_map<
        EntityComponentKey,
        std::unordered_set<std::any, AnyHash>,
        EntityComponentKeyHash
    > listeners;

    std::unordered_map<Entity, std::unordered_set<ComponentId>> observedComponents;


    template <typename Comp>
    static inline EntityComponentKey CreateKey(Entity entity)
        { return EntityComponentKey{ .entity=entity, .compId=ComponentsManager::GetComponentId<Comp>() }; }
};
