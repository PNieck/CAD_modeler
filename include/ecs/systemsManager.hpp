#pragma once

#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <memory>
#include <algorithm>

#include "system.hpp"
#include "componentsManager.hpp"
#include "systemConcept.hpp"


using SystemId = std::size_t;


class SystemsManager {
public:
    template<SystemConcept Sys>
    void RegisterSystem() {
        SystemId id = GetSystemID<Sys>();
        systems.insert({ id, std::make_shared<Sys>() });
    }


    template<SystemConcept Sys>
    std::shared_ptr<Sys> GetSystem() const {
        return std::static_pointer_cast<Sys>(systems.at(GetSystemID<Sys>()));
    }


    template<SystemConcept Sys>
    static constexpr SystemId GetSystemID() {
        return typeid(Sys).hash_code();
    }


    template<SystemConcept Sys, typename Comp>
    void RegisterRequiredComponent() {
        const ComponentId compId = ComponentsManager::GetComponentId<Comp>();
        const SystemId sysId = GetSystemID<Sys>();

        componentsToSystemsMap[compId].insert(sysId);

        requiredComponents[sysId].insert(compId);
    }


    template<typename Comp>
    void EntityGainedComponent(const Entity entity, const std::set<ComponentId>& components) {
        const ComponentId compId = ComponentsManager::GetComponentId<Comp>();

        for (const auto systemId : componentsToSystemsMap[compId]) {
            if (std::ranges::includes(components, requiredComponents[systemId])) {
                systems[systemId]->AddEntity(entity);
            }
        }
    }


    template<typename Comp>
    void EntityLostComponent(const Entity entity) {
        const ComponentId compId = ComponentsManager::GetComponentId<Comp>();

        for (const auto systemId : componentsToSystemsMap[compId]) {
            systems[systemId]->RemoveEntity(entity);
        }
    }


    void EntityDeleted(const Entity entity) {
        for (const auto &val: systems | std::views::values) {
            val->RemoveEntity(entity);
        }
    }

private:
    std::unordered_map<ComponentId, std::unordered_set<SystemId>>
        componentsToSystemsMap;

    std::map<SystemId, std::set<ComponentId>> requiredComponents;

    std::unordered_map<SystemId, std::shared_ptr<System>> systems;
};
