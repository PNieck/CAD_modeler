#include <CAD_modeler/model/systems/controlPointsRegistrySystem.hpp>


void ControlPointsRegistrySystem::UnregisterControlPoint(Entity owner, Entity cp, SystemId system)
{
    if (!controlPointsOwners.contains(cp))
        return;

    auto& ownersSet = controlPointsOwners.at(cp);
    ownersSet.erase({owner, system});

    if (ownersSet.empty())
        controlPointsOwners.erase(cp);
}
