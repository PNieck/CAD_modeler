#pragma once

#include "entitiesManager.hpp"
#include "components/componentsManager.hpp"
#include "systems/systemsManager.hpp"

#include <memory>


class Coordinator {
public:
    template<typename Comp>
    inline void RegisterComponent() {
        componentMgr.RegisterComponent<Comp>();
    }


    template<typename Sys>
    void RegisterSystem() {
        systemsMgr.RegisterSystem<Sys>();
        auto const& system = systemsMgr.GetSystem<Sys>();
        system->SetCoordinator(this);
    }


    template<typename Sys, typename Comp>
    inline void RegisterRequiredComponent() {
        systemsMgr.RegisterRequiredComponent<Sys, Comp>();
    }


    inline Entity CreateEntity() {
        return entitiesMgr.CreateEntity();
    }


    void DestroyEntity(Entity entity) {
        componentMgr.EntityDeleted(entity);
        systemsMgr.EntityDeleted(entity);
        entitiesMgr.DestroyEntity(entity);
    }


    template<typename Comp>
    void AddComponent(Entity entity, const Comp& component) {
        componentMgr.AddComponent<Comp>(entity, component);

        auto const& componentsSet = componentMgr.GetEntityComponents(entity);
        systemsMgr.EntityGainedComponent<Comp>(entity, componentsSet);
    }


    template<typename Comp>
    void DeleteComponent(Entity entity) {
        componentMgr.DeleteComponent<Comp>(entity);
        systemsMgr.EntityLostComponent<Comp>(entity);
    }


    template<typename Comp>
    inline Comp& GetComponent(Entity entity) {
        return componentMgr.GetComponent<Comp>(entity);
    }

    template<typename Comp>
    inline const Comp& GetConstComponent(Entity entity) const {
        return componentMgr.GetComponent<Comp>(entity);
    }

    inline const std::set<ComponentId>& GetEntityComponents(Entity entity) const
        { return componentMgr.GetEntityComponents(entity); }

    template<typename Sys>
    inline std::shared_ptr<Sys> GetSystem() {
        return systemsMgr.GetSystem<Sys>();
    }


private:
    ComponentsManager componentMgr;
    EntitiesManager entitiesMgr;
    SystemsManager systemsMgr;
};
