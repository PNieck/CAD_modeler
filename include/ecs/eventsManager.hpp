#pragma once

#include "entitiesManager.hpp"
#include "componentsManager.hpp"
#include "idManager.hpp"

#include <any>
#include <functional>
#include <unordered_map>


using HandlerId = Id;


class EventsManager {
public:
    template <typename Comp>
    HandlerId SubscribeToComponentChange(Entity entity, const std::function<void(Entity, const Comp&)>& function) {
        HandlerId newHandlerId = handlersIdManager.CreateNewId();
        GetHandlers<Comp>(entity).insert({ newHandlerId, function });

        return newHandlerId;
    }


    template <typename Comp>
    inline void UnsubscribeToComponentChange(Entity entity, HandlerId handlerId) {
        GetHandlers<Comp>(entity).erase(handlerId);
    }


    template <typename Comp>
    void ComponentChanged(Entity entity, const Comp& component) {
        auto const& handlers = GetHandlers<Comp>(entity);

        for(auto const& pair: handlers) {
            auto function = std::any_cast<std::function<void(Entity, const Comp&)>>(pair.second);

            function(entity, component);
        }
    }


    void EntityDeleted(Entity entity) {
        for (auto const& pairs: listeners[entity]) {
            for (auto const& handler: pairs.second) {
                handlersIdManager.DestroyId(handler.first);
            }
        }

        listeners.erase(entity);
    }


private:
    std::unordered_map<
        Entity,
        std::unordered_map<
            ComponentId,
            std::unordered_map<
                HandlerId,
                std::any
            >
        >
    > listeners;

    IdManager handlersIdManager;


    template <typename Comp>
    std::unordered_map<HandlerId, std::any>& GetHandlers(Entity entity) {
        ComponentId compId = ComponentsManager::GetComponentId<Comp>();
        return listeners[entity][compId];
    }
};
