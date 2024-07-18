#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "utils/nameGenerator.hpp"
#include "../components/position.hpp"
#include "../components/curveControlPoints.hpp"
#include "../components/c0SurfacePatches.hpp"
#include "../components/c0SurfaceDensity.hpp"
#include "c0PatchesSystem.hpp"


class C0SurfaceSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    Entity CreateSurface(const Position& pos, const alg::Vec3& direction, float length, float width);

    void AddRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;
    void AddColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

    void DeleteRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;
    void DeleteColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

    inline void SetDensity(Entity entity, C0SurfaceDensity density) const
        { coordinator->SetComponent<C0SurfaceDensity>(entity, density); }

    inline void ShowBezierPolygon(Entity cylinder) const
        { coordinator->GetSystem<C0PatchesSystem>()->ShowPolygon(cylinder); }

    inline void HideBezierPolygon(Entity cylinder) const
        { coordinator->GetSystem<C0PatchesSystem>()->HidePolygon(cylinder); }

    void Recalculate(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

private:
    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;
    NameGenerator nameGenerator;


    class DeletionHandler: public EventHandler<C0SurfacePatches> {
    public:
        DeletionHandler(Coordinator& coordinator):
            coordinator(coordinator) {}

        void HandleEvent(Entity entity, const C0SurfacePatches& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
    };
};
