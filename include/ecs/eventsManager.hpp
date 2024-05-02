#pragma once

#include "entitiesManager.hpp"
#include "componentsManager.hpp"
#include "eventHandler.hpp"
#include "idManager.hpp"

#include <any>
#include <functional>
#include <unordered_map>
#include <optional>
#include <stack>
#include <memory>


enum class EventType {
    NewComponent,
    ComponentChanged,
    ComponentDeleted,
};


using HandlerId = Id;


class EventsManager {
public:
    EventsManager(ComponentsManager& compMgr):
        componentsMgr(compMgr) {}

    template <typename Comp>
    void RegisterComponent() {
        componentDeletionFunctions.insert(
            {ComponentsManager::GetComponentId<Comp>(), &EventsManager::ComponentDeleted<Comp>}
        );
    }


    template <typename Comp>
    HandlerId Subscribe(Entity entity, std::shared_ptr<EventHandler<Comp>> handler) {
        HandlerId newHandlerId = handlersIdManager.CreateNewId();
        listeners[entity][ComponentsManager::GetComponentId<Comp>()].insert({ newHandlerId, handler });

        return newHandlerId;
    }


    template <typename Comp>
    void Unsubscribe(Entity entity, HandlerId handlerId) {
        auto handlers = GetHandlers(entity, ComponentsManager::GetComponentId<Comp>());

        if (handlers != nullptr)
            handlers->erase(handlerId);
    }


    template <typename Comp>
    std::shared_ptr<EventHandler<Comp>> GetHandler(Entity entity, HandlerId handlerId) {
        auto handlers = GetHandlers(entity, ComponentsManager::GetComponentId<Comp>());

        return std::static_pointer_cast<EventHandler<Comp>>(handlers->at(handlerId));
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
                std::shared_ptr<void>
            >
        >
    > listeners;

    std::unordered_map<ComponentId, std::function<void(EventsManager&, Entity)>> componentDeletionFunctions;

    IdManager handlersIdManager;

    ComponentsManager& componentsMgr;

    std::unordered_map<HandlerId, std::shared_ptr<void>>* GetHandlers(Entity entity, ComponentId compId) {
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

        std::stack<std::shared_ptr<void>> handlersStack;

        for (auto handler: *handlers) {
            handlersStack.push(handler.second);
        }

        while (!handlersStack.empty()) {
            auto function = std::static_pointer_cast<EventHandler<Comp>>(handlersStack.top());
            function->HandleEvent(entity, component, type);

            handlersStack.pop();
        }
    }
};
