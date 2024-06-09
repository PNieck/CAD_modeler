#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "shaders/shaderRepository.hpp"
#include "utils/nameGenerator.hpp"
#include "../components/position.hpp"
#include "../components/c0SurfaceDensity.hpp"
#include "c0PatchesSystem.hpp"


class C0CylinderSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity CreateCylinder(const Position& pos, const alg::Vec3& direction, float radius);
    
    void AddRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;
    void AddColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;

    void DeleteRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;
    void DeleteColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;

    inline void ShowBezierPolygon(Entity cylinder) const
        { coordinator->GetSystem<C0PatchesSystem>()->ShowPolygon(cylinder); }

    inline void HideBezierPolygon(Entity cylinder) const
        { coordinator->GetSystem<C0PatchesSystem>()->HidePolygon(cylinder); }

    inline void SetDensity(Entity entity, C0SurfaceDensity density) const
        { coordinator->SetComponent<C0SurfaceDensity>(entity, density); }

private:
    NameGenerator nameGenerator;

    void RecalculatePositions(Entity cylinder, const Position& pos, const alg::Vec3& direction, float radius) const;
};
