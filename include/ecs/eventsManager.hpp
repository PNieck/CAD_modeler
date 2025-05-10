#pragma once

#include "entitiesManager.hpp"
#include "componentsManager.hpp"
#include "eventHandler.hpp"
#include "idManager.hpp"

#include <functional>
#include <unordered_map>
#include <stack>
#include <memory>
#include <ranges>


enum class EventType {
    NewComponent,
    ComponentChanged,
    ComponentDeleted,
};


using HandlerId = Id;


class EventsManager {
public:
    explicit EventsManager(ComponentsManager& compMgr):
        componentsMgr(compMgr) {}

    template <typename Comp>
    void RegisterComponent() {
        componentDeletionFunctions.insert(
            {ComponentsManager::GetComponentId<Comp>(), &EventsManager::ComponentDeleted<Comp>}
        );
    }


    template <typename Comp>
    HandlerId Subscribe(const Entity entity, std::shared_ptr<EventHandler<Comp>> handler) {
        HandlerId newHandlerId = handlersIdManager.CreateNewId();
        listeners[entity][ComponentsManager::GetComponentId<Comp>()].insert({ newHandlerId, handler });

        return newHandlerId;
    }


    template <typename Comp>
    void Unsubscribe(const Entity entity, HandlerId handlerId) {
        auto handlers = GetHandlers(entity, ComponentsManager::GetComponentId<Comp>());

        if (handlers != nullptr)
            handlers->erase(handlerId);
    }


    template <typename Comp>
    std::shared_ptr<EventHandler<Comp>> GetHandler(const Entity entity, HandlerId handlerId) {
        auto handlers = GetHandlers(entity, ComponentsManager::GetComponentId<Comp>());

        return std::static_pointer_cast<EventHandler<Comp>>(handlers->at(handlerId));
    }


    template <typename Comp>
    void ComponentChanged(const Entity entity, const Comp& component)
        { ComponentEvent<Comp>(entity, component, EventType::ComponentChanged); }


    template <typename Comp>
    void ComponentDeleted(const Entity entity)
        { ComponentEvent<Comp>(entity, componentsMgr.GetComponent<Comp>(entity), EventType::ComponentDeleted); }


    template <typename Comp>
    void ComponentAdded(const Entity entity, const Comp& component)
        { ComponentEvent<Comp>(entity, component, EventType::NewComponent); }


    void EntityDeleted(const Entity entity) {
        // Run events for all components deletion
        for (auto compId: componentsMgr.GetEntityComponents(entity)) {
            componentDeletionFunctions.at(compId)(*this, entity);
        }

        for (const auto &val: listeners[entity] | std::views::values) {
            for (const auto &key: val | std::views::keys) {
                handlersIdManager.DestroyId(key);
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

    std::unordered_map<HandlerId, std::shared_ptr<void>>* GetHandlers(const Entity entity, const ComponentId compId) {
        const auto it = listeners.find(entity);
        if (it == listeners.end())
            return nullptr;

        const auto it2 = it->second.find(compId);
        if (it2 == it->second.end())
            return nullptr;

        return &it2->second;
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
