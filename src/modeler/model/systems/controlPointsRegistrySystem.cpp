#include <CAD_modeler/model/systems/controlPointsRegistrySystem.hpp>


void ControlPointsRegistrySystem::UnregisterControlPoint(const Entity owner, const Entity cp, const SystemId system)
{
    const auto it = controlPointsOwners.find(cp);
    if (it == controlPointsOwners.end())
        return;

    auto& ownersSet = controlPointsOwners.at(cp);
    ownersSet.erase({owner, system});

    if (ownersSet.empty())
        controlPointsOwners.erase(cp);
    auto& ownersSet = it->second;
    if (ownersSet.erase({owner, system}) > 0)
        if (ownersSet.empty())
            controlPointsOwners.erase(it);
}
}
