#pragma once

#include <ecs/coordinator.hpp>

#include "../surfaceSystem.hpp"
#include "CAD_modeler/model/systems/c0PatchesSystem.hpp"
#include "CAD_modeler/model/systems/c2SurfacesSystem.hpp"
#include "CAD_modeler/model/systems/toriSystem.hpp"


inline std::shared_ptr<SurfaceSystem> GetSurfaceSystem(const Coordinator& coordinator, const Entity entity) {
    std::shared_ptr<SurfaceSystem> result = coordinator.GetSystem<C0PatchesSystem>();
    if (result->HasEntity(entity))
        return result;

    result = coordinator.GetSystem<C2SurfaceSystem>();
    if (result->HasEntity(entity))
        return result;

    result = coordinator.GetSystem<ToriSystem>();
    if (result->HasEntity(entity))
        return result;

    throw std::runtime_error("Entity is not a surface");
}
