#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <optional>

#include "../../utilities/hashCombine.hpp"


class ControlPointsRegistrySystem: public System {
public:
    using Owner = std::tuple<Entity, SystemId>;
private:
    struct OwnerHash {
        size_t operator() (const Owner& owner) const {
            return stdh::hashCombine(
                std::hash<Entity>()(std::get<0>(owner)),
                std::hash<SystemId>()(std::get<1>(owner))
            );
        }
    };
public:
    
    using OwnersSet = std::unordered_set<Owner, OwnerHash>;

    static void RegisterSystem(Coordinator& coordinator)
        { coordinator.RegisterSystem<ControlPointsRegistrySystem>(); }

    void RegisterControlPoint(Entity owner, const Entity cp, SystemId system)
        { controlPointsOwners[cp].insert({owner, system}); }

    void UnregisterControlPoint(Entity owner, Entity cp, SystemId system);

    bool IsAControlPoint(const Entity cp) const
        { return controlPointsOwners.contains(cp); }

    const OwnersSet& GetOwnersOfControlPoints(const Entity cp) const
        { return controlPointsOwners.at(cp); }

private:
    std::unordered_map<Entity, OwnersSet> controlPointsOwners;
};
