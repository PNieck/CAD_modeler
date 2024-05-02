#pragma once

#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <memory>
#include <algorithm>

#include "system.hpp"
#include "componentsManager.hpp"


using SystemId = std::size_t;


class SystemsManager {
public:
    template<typename Sys>
    void RegisterSystem() {
        SystemId id = GetSystemID<Sys>();
        systems.insert({ id, std::make_shared<Sys>() });
    }


    template<typename Sys>
    std::shared_ptr<Sys> GetSystem() const {
        return std::static_pointer_cast<Sys>(systems.at(GetSystemID<Sys>()));
    }


    template<typename Sys>
    static constexpr SystemId GetSystemID() {
        return typeid(Sys).hash_code();
    }


    template<typename Sys, typename Comp>
    void RegisterRequiredComponent() {
        ComponentId compId = ComponentsManager::GetComponentId<Comp>();
        SystemId sysId = GetSystemID<Sys>();

        componentsToSystemsMap[compId].insert(sysId);

        requiredComponents[sysId].insert(compId);
    }


    template<typename Comp>
    void EntityGainedComponent(Entity entity, const std::set<ComponentId>& components) {
        ComponentId compId = ComponentsManager::GetComponentId<Comp>();

        auto const& systemsIds = componentsToSystemsMap[compId];

        for (const auto systemId : systemsIds) {
            const auto& requirements = requiredComponents[systemId];

            if (std::includes(
                components.cbegin(), components.cend(),
                requirements.cbegin(), requirements.cend()
            )) {
                systems[systemId]->AddEntity(entity);
            }
        }
    }


    template<typename Comp>
    void EntityLostComponent(Entity entity) {
        ComponentId compId = ComponentsManager::GetComponentId<Comp>();

        auto const& systemsIds = componentsToSystemsMap[compId];

        for (const auto systemId : systemsIds) {
            systems[systemId]->RemoveEntity(entity);
        }
    }


    void EntityDeleted(Entity entity) {
        for (auto const& pair : systems) {
            pair.second->RemoveEntity(entity);
        }
    }

private:
    std::unordered_map<ComponentId, std::unordered_set<SystemId>>
        componentsToSystemsMap;

    std::map<SystemId, std::set<ComponentId>> requiredComponents;

    std::unordered_map<SystemId, std::shared_ptr<System>> systems;
};
