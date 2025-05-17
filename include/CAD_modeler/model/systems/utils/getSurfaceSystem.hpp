#pragma once

#include <ecs/coordinator.hpp>

#include "CAD_modeler/model/systems/c0PatchesSystem.hpp"
#include "CAD_modeler/model/systems/c2PatchesSystem.hpp"
#include "CAD_modeler/model/systems/toriSystem.hpp"


inline std::shared_ptr<SurfaceSystem> GetSurfaceSystem(const Coordinator& coordinator, const Entity entity) {
    std::shared_ptr<SurfaceSystem> result = coordinator.GetSystem<C0PatchesSystem>();
    if (result->HasEntity(entity))
        return result;

    result = coordinator.GetSystem<C2PatchesSystem>();
    if (result->HasEntity(entity))
        return result;

    result = coordinator.GetSystem<ToriSystem>();
    if (result->HasEntity(entity))
        return result;

    throw std::runtime_error("Entity is not a surface");
}
