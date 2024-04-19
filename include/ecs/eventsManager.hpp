#pragma once

#include "entitiesManager.hpp"
#include "componentsManager.hpp"
#include "idManager.hpp"

#include <any>
#include <functional>
#include <unordered_map>
#include <optional>
#include <stack>


using HandlerId = Id;


enum class EventType {
    NewComponent,
    ComponentChanged,
    ComponentDeleted,
};


class EventsManager {
public:
    EventsManager(ComponentsManager& compMgr):
        componentsMgr(compMgr) {}

    template <typename Comp>
    void RegisterComponent() {
        componentDeletionFunctions.insert({ComponentsManager::GetComponentId<Comp>(), &EventsManager::ComponentDeleted<Comp>});
    }


    template <typename Comp>
    HandlerId Subscribe(Entity entity, const std::function<void(Entity, const Comp&, EventType)>& function) {
        HandlerId newHandlerId = handlersIdManager.CreateNewId();
        listeners[entity][ComponentsManager::GetComponentId<Comp>()].insert({ newHandlerId, function });

        return newHandlerId;
    }


    template <typename Comp>
    inline void Unsubscribe(Entity entity, HandlerId handlerId) {
        //GetHandlers<Comp>(entity).erase(handlerId);
        HandlersToUnsubscribe.push(std::make_tuple(entity, ComponentsManager::GetComponentId<Comp>(), handlerId));
    }


    template <typename Comp>
    inline void ComponentChanged(Entity entity, const Comp& component)
        { ComponentEvent<Comp>(entity, component, EventType::ComponentChanged); }


    template <typename Comp>
    void ComponentDeleted(Entity entity)
        { ComponentEvent<Comp>(entity, componentsMgr.GetComponent<Comp>(entity), EventType::ComponentDeleted); }


    template <typename Comp>
    inline void ComponentAdded(Entity entity, const Comp& component)
        { ComponentEvent<Comp>(entity, component, EventType::NewComponent); }


    void EntityDeleted(Entity entity) {
        auto const& components = componentsMgr.GetEntityComponents(entity);

        // Run events for all components deletion
        for (auto compId: components) {
            componentDeletionFunctions.at(compId)(*this, entity);
        }

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

    std::unordered_map<ComponentId, std::function<void(EventsManager&, Entity)>> componentDeletionFunctions;

    IdManager handlersIdManager;

    ComponentsManager& componentsMgr;

    std::stack<std::tuple<Entity, ComponentId, HandlerId>> HandlersToUnsubscribe;
    unsigned int eventResolvingDepth = 0;

    std::unordered_map<HandlerId, std::any>* GetHandlers(Entity entity, ComponentId compId) {
        auto it = listeners.find(entity);
        if (it == listeners.end())
            return nullptr;

        auto it2 = (*it).second.find(compId);
        if (it2 == (*it).second.end())
            return nullptr;

        return &((*it2).second);
    }

    template <typename Comp>
    void ComponentEvent(Entity entity, const Comp& component, EventType type) {
        auto handlers = GetHandlers(entity, ComponentsManager::GetComponentId<Comp>());

        if (handlers == nullptr)
            return;

        if (eventResolvingDepth++ == 0)
            FlushUnsubscribedHandlersBuffer();

        for(auto const& pair: *handlers) {
            auto function = std::any_cast<std::function<void(Entity, const Comp&, EventType)>>(pair.second);

            function(entity, component, type);
        }

        eventResolvingDepth--;
    }


    void FlushUnsubscribedHandlersBuffer() {
        while (!HandlersToUnsubscribe.empty()) {
            auto& handleTuple = HandlersToUnsubscribe.top();

            auto handlers = GetHandlers(std::get<0>(handleTuple), std::get<1>(handleTuple));
            if (handlers == nullptr) {
                HandlersToUnsubscribe.pop();
                continue;
            }

            handlers->erase(std::get<2>(handleTuple));
            HandlersToUnsubscribe.pop();
        }
    }
};
