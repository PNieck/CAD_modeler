#pragma once

#include <ecs/coordinator.hpp>

#include "CAD_modeler/model/systems/c0PatchesSystem.hpp"
#include "CAD_modeler/model/systems/c2PatchesSystem.hpp"
#include "CAD_modeler/model/systems/toriSystem.hpp"
#include "CAD_modeler/model/systems/equidistanceC2SurfaceSystem.hpp"
#include "CAD_modeler/model/systems/equidistanceC0SurfaceSystem.hpp"

inline std::shared_ptr<SurfaceSystem> GetSurfaceSystem(const Coordinator& coordinator, const Entity entity) {
    std::shared_ptr<SurfaceSystem> result = coordinator.GetSystem<C0PatchesSystem>();
    if (result->HasEntity(entity))
        return result;

    result = coordinator.GetSystem<C2PatchesSystem>();
    if (result->HasEntity(entity))
        return result;

    if (coordinator.SystemRegistered<ToriSystem>()) {
        result = coordinator.GetSystem<ToriSystem>();
        if (result->HasEntity(entity))
            return result;
    }

    if (coordinator.SystemRegistered<EquidistanceC2SurfaceSystem>()) {
        result = coordinator.GetSystem<EquidistanceC2SurfaceSystem>();
        if (result->HasEntity(entity))
            return result;
    }

    if (coordinator.SystemRegistered<EquidistanceC0SurfaceSystem>()) {
        result = coordinator.GetSystem<EquidistanceC0SurfaceSystem>();
        if (result->HasEntity(entity))
            return result;
    }

    throw std::runtime_error("Entity is not a surface");
}
