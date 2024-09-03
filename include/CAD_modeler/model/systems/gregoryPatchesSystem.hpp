#pragma once

#include "../components/c0Patches.hpp"

#include <ecs/system.hpp>

#include <vector>


class GregoryPatchesSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    std::vector<std::vector<Entity>> FindHoleToFill(const std::vector<C0Patches>& patches) const;
};
