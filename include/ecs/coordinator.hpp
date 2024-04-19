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


    template <typename Sys>
    void RegisterSystem() {
        systemsMgr.RegisterSystem<Sys>();
        auto const& system = systemsMgr.GetSystem<Sys>();
        system->SetCoordinator(this);
    }


    template <typename Sys, typename Comp>
    inline void RegisterRequiredComponent() {
        systemsMgr.RegisterRequiredComponent<Sys, Comp>();
    }


    inline Entity CreateEntity() {
        return entitiesMgr.CreateEntity();
    }


    void DestroyEntity(Entity entity) {
        eventMgr.EntityDeleted(entity);
        componentMgr.EntityDeleted(entity);
        systemsMgr.EntityDeleted(entity);
        entitiesMgr.DestroyEntity(entity);
    }


    template <typename Comp>
    void AddComponent(Entity entity, const Comp& component) {
        componentMgr.AddComponent<Comp>(entity, component);

        auto const& componentsSet = componentMgr.GetEntityComponents(entity);
        systemsMgr.EntityGainedComponent<Comp>(entity, componentsSet);

        eventMgr.ComponentAdded<Comp>(entity, component);
    }


    template <typename Comp>
    void DeleteComponent(Entity entity) {
        eventMgr.ComponentDeleted<Comp>(entity);
        componentMgr.DeleteComponent<Comp>(entity);
        systemsMgr.EntityLostComponent<Comp>(entity);
    }


    template <typename Comp>
    inline const Comp& GetComponent(Entity entity) const {
        return componentMgr.GetComponent<Comp>(entity);
    }

    template <typename Comp>
    void SetComponent(Entity entity, const Comp& component) {
        componentMgr.GetComponent<Comp>(entity) = component;
        eventMgr.ComponentChanged<Comp>(entity, component);
    }

    template <typename Comp>
    void EditComponent(Entity entity, std::function<void(Comp& component)> func) {
        Comp& component = componentMgr.GetComponent<Comp>(entity);
        func(component);
        eventMgr.ComponentChanged<Comp>(entity, component);
    }

    template <typename Comp>
    inline HandlerId Subscribe(Entity entity, std::function<void(Entity, const Comp&, EventType)> function)
        { return eventMgr.Subscribe<Comp>(entity, function); }

    template <typename Comp>
    inline void Unsubscribe(Entity entity, HandlerId handlerId)
        { eventMgr.Unsubscribe<Comp>(entity, handlerId); }

    inline const std::set<ComponentId>& GetEntityComponents(Entity entity) const
        { return componentMgr.GetEntityComponents(entity); }

    template <typename Sys>
    inline std::shared_ptr<Sys> GetSystem() {
        return systemsMgr.GetSystem<Sys>();
    }


private:
    ComponentsManager componentMgr;
    EntitiesManager entitiesMgr;
    SystemsManager systemsMgr;
    EventsManager eventMgr;
};
