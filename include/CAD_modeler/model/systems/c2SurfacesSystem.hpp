#pragma once

#include <ecs/coordinator.hpp>

#include "../components/position.hpp"
#include "../components/c2Patches.hpp"
#include "../components/patchesDensity.hpp"

#include "controlNetSystem.hpp"
#include "surfaceSystem.hpp"


class C2SurfaceSystem: public SurfaceSystem {
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

    void SetDensity(const Entity entity, const PatchesDensity density) const
        { coordinator->SetComponent<PatchesDensity>(entity, density); }

    int GetRowsCnt(const Entity surface) const
        { return coordinator->GetComponent<C2Patches>(surface).PatchesInRow(); }

    int GetColsCnt(const Entity surface) const
        { return coordinator->GetComponent<C2Patches>(surface).PatchesInCol(); }

    void ShowDeBoorNet(Entity surface);

    void HideDeBoorNet(const Entity surface) const
        { coordinator->GetSystem<ControlNetSystem>()->DeleteControlPointsNet(surface); }

    void Recalculate(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

    Position PointOnSurface(const C2Patches& patches, float u, float v) const;
    Position PointOnSurface(const Entity entity, const float u, const float v) const override
        { return PointOnSurface(coordinator->GetComponent<C2Patches>(entity), u, v); }

    alg::Vec3 PartialDerivativeU(const C2Patches& patches, float u, float v) const;
    alg::Vec3 PartialDerivativeU(const Entity entity, const float u, const float v) const override
        { return PartialDerivativeU(coordinator->GetComponent<C2Patches>(entity), u, v); }

    alg::Vec3 PartialDerivativeV(const C2Patches& patches, float u, float v) const;
    alg::Vec3 PartialDerivativeV(const Entity entity, const float u, const float v) const override
        { return PartialDerivativeV(coordinator->GetComponent<C2Patches>(entity), u, v); }

    static float MaxU(const C2Patches& patches)
        { return static_cast<float>(patches.PatchesInRow()); }

    float MaxU(const Entity entity) const override
        { return MaxU(coordinator->GetComponent<C2Patches>(entity)); }

    static float MaxV(const C2Patches& patches)
        { return static_cast<float>(patches.PatchesInCol()); }

    float MaxV(const Entity entity) const override
        { return MaxV(coordinator->GetComponent<C2Patches>(entity)); }

    void Render(const alg::Mat4x4& cameraMtx) const;

    void Update() const;

private:
    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;
    
    void UpdateMesh(Entity surface, const C2Patches& patches) const;

    std::vector<float> GenerateVertices(const C2Patches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C2Patches& patches) const;

    static void NormalizeUV(const C2Patches& patches, float& u, float& v);

    class DeletionHandler final : public EventHandler<C2Patches> {
    public:
        explicit DeletionHandler(Coordinator& coordinator):
            coordinator(coordinator) {}

        void HandleEvent(Entity entity, const C2Patches& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
    };

    class ControlPointMovedHandler final : public EventHandler<Position> {
    public:
        ControlPointMovedHandler(const Entity targetObject, Coordinator& coordinator):
            coordinator(coordinator), targetObject(targetObject) {}

        void HandleEvent(Entity entity, const Position& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
        Entity targetObject;
    };
};
