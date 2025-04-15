#pragma once

#include "entitiesManager.hpp"
#include "componentsManager.hpp"
#include "systemsManager.hpp"
#include "eventsManager.hpp"

#include <memory>
#include <functional>


class Coordinator {
public:
    Coordinator():
        componentMgr(), entitiesMgr(), systemsMgr(), eventMgr(componentMgr) {}

    template <typename Comp>
    void RegisterComponent() {
        componentMgr.RegisterComponent<Comp>();
        eventMgr.RegisterComponent<Comp>();
    }


    template <SystemConcept Sys>
    void RegisterSystem() {
        systemsMgr.RegisterSystem<Sys>();
        auto const& system = systemsMgr.GetSystem<Sys>();
        system->SetCoordinator(this);
    }


    template <SystemConcept Sys, typename Comp>
    void RegisterRequiredComponent() {
        systemsMgr.RegisterRequiredComponent<Sys, Comp>();
    }


    Entity CreateEntity() {
        const Entity result = entitiesMgr.CreateEntity();
        componentMgr.RegisterNewEntity(result);

        return result;
    }


    void DestroyEntity(const Entity entity) {
        eventMgr.EntityDeleted(entity);
        componentMgr.EntityDeleted(entity);
        systemsMgr.EntityDeleted(entity);
        entitiesMgr.DestroyEntity(entity);
    }


    template <typename Comp>
    void AddComponent(const Entity entity, const Comp& component) {
        componentMgr.AddComponent<Comp>(entity, component);

        auto const& componentsSet = componentMgr.GetEntityComponents(entity);
        systemsMgr.EntityGainedComponent<Comp>(entity, componentsSet);

        eventMgr.ComponentAdded<Comp>(entity, component);
    }


    template <typename Comp>
    void AddComponent(const Entity entity, Comp&& component) {
        componentMgr.AddComponent<Comp>(entity, component);

        auto const& componentsSet = componentMgr.GetEntityComponents(entity);
        systemsMgr.EntityGainedComponent<Comp>(entity, componentsSet);

        eventMgr.ComponentAdded<Comp>(entity, component);
    }


    template <typename Comp>
    void DeleteComponent(const Entity entity) {
        eventMgr.ComponentDeleted<Comp>(entity);
        componentMgr.DeleteComponent<Comp>(entity);
        systemsMgr.EntityLostComponent<Comp>(entity);
    }


    template <typename Comp>
    const Comp& GetComponent(const Entity entity) const {
        return componentMgr.GetComponent<Comp>(entity);
    }

    template <typename Comp>
    void SetComponent(const Entity entity, const Comp& component) {
        componentMgr.GetComponent<Comp>(entity) = component;
        eventMgr.ComponentChanged<Comp>(entity, component);
    }

    template <typename Comp>
    void EditComponent(const Entity entity, std::function<void(Comp& component)> func) {
        Comp& component = componentMgr.GetComponent<Comp>(entity);
        func(component);
        eventMgr.ComponentChanged<Comp>(entity, component);
    }

    template <typename Comp>
    HandlerId Subscribe(const Entity entity, std::shared_ptr<EventHandler<Comp>> function)
        { return eventMgr.Subscribe<Comp>(entity, function); }

    template <typename Comp>
    void Unsubscribe(const Entity entity, const HandlerId handlerId)
        { eventMgr.Unsubscribe<Comp>(entity, handlerId); }

    template <typename Comp>
    std::shared_ptr<EventHandler<Comp>> GetEventHandler(const Entity entity, const HandlerId handlerId)
        { return eventMgr.GetHandler<Comp>(entity, handlerId); }

    const std::set<ComponentId>& GetEntityComponents(const Entity entity) const
        { return componentMgr.GetEntityComponents(entity); }

    template <typename Comp>
    bool HasComponent(const Entity entity) const
        { return componentMgr.GetEntityComponents(entity).contains(ComponentsManager::GetComponentId<Comp>()); }

    template <SystemConcept Sys>
    std::shared_ptr<Sys> GetSystem() const {
        return systemsMgr.GetSystem<Sys>();
    }

    template <typename Comp>
    static constexpr ComponentId GetComponentID() {
        return ComponentsManager::GetComponentId<Comp>();
    }


    template <SystemConcept Sys>
    static constexpr SystemId GetSystemID() {
        return SystemsManager::GetSystemID<Sys>();
    }


private:
    ComponentsManager componentMgr;
    EntitiesManager entitiesMgr;
    SystemsManager systemsMgr;
    EventsManager eventMgr;
};
