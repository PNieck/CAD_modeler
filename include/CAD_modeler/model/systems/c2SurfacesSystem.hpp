#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "../components/position.hpp"
#include "../components/c2Patches.hpp"
#include "../components/patchesDensity.hpp"
#include "controlNetSystem.hpp"


class C2SurfaceSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    Entity CreateSurface(const Position& pos, const alg::Vec3& direction, float length, float width);

    Entity CreateSurface(C2Patches& patches);

    void AddRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;
    void AddColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

    void DeleteRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;
    void DeleteColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

    void MergeControlPoints(Entity cylinder, Entity oldCP, Entity newCP);

    inline void SetDensity(Entity entity, PatchesDensity density) const
        { coordinator->SetComponent<PatchesDensity>(entity, density); }

    inline int GetRowsCnt(Entity surface) const
        { return coordinator->GetComponent<C2Patches>(surface).PatchesInRow(); }

    inline int GetColsCnt(Entity surface) const
        { return coordinator->GetComponent<C2Patches>(surface).PatchesInCol(); }

    void ShowDeBoorNet(Entity surface);

    inline void HideDeBoorNet(Entity surface)
        { coordinator->GetSystem<ControlNetSystem>()->DeleteControlPointsNet(surface); }

    void Recalculate(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

    void Render(const alg::Mat4x4& cameraMtx) const;

    void Update() const;

private:
    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;
    
    void UpdateMesh(Entity surface, const C2Patches& patches) const;

    std::vector<float> GenerateVertices(const C2Patches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C2Patches& patches) const;


    class DeletionHandler: public EventHandler<C2Patches> {
    public:
        DeletionHandler(Coordinator& coordinator):
            coordinator(coordinator) {}

        void HandleEvent(Entity entity, const C2Patches& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
    };

    class ControlPointMovedHandler: public EventHandler<Position> {
    public:
        ControlPointMovedHandler(Entity targetObject, Coordinator& coordinator):
            coordinator(coordinator), targetObject(targetObject) {}

        void HandleEvent(Entity entity, const Position& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
        Entity targetObject;
    };
};
