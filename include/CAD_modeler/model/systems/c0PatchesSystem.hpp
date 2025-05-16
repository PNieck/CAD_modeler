#pragma once

#include <ecs/coordinator.hpp>

#include "surfaceSystem.hpp"
#include "controlNetSystem.hpp"

#include "../components/c0Patches.hpp"
#include "../components/position.hpp"


class C0PatchesSystem final : public SurfaceSystem {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    Entity CreateCylinder(const Position& pos, const alg::Vec3& direction, float radius);

    void AddRowOfCylinderPatches(Entity cylinder, const Position& pos, const alg::Vec3& direction, float radius) const;
    void AddColOfCylinderPatches(Entity cylinder, const Position& pos, const alg::Vec3& direction, float radius) const;

    void DeleteRowOfCylinderPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;
    void DeleteColOfCylinderPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;

    void RecalculateCylinder(Entity cylinder, const Position& pos, const alg::Vec3& direction, float radius) const;

    int GetRowsCnt(const Entity surface) const
        { return coordinator->GetComponent<C0Patches>(surface).PatchesInRow(); }

    int GetColsCnt(const Entity surface) const
        { return coordinator->GetComponent<C0Patches>(surface).PatchesInCol(); }

    void MergeControlPoints(Entity surface, Entity oldCP, Entity newCP, SystemId system);

    void ShowBezierNet(Entity surface);

    void HideBezierNet(const Entity surface) const
        { coordinator->GetSystem<ControlNetSystem>()->DeleteControlPointsNet(surface); }

    Position PointOnSurface(const C0Patches& patches, float u, float v) const;
    Position PointOnSurface(const Entity entity, const float u, const float v) const override
        { return PointOnSurface(coordinator->GetComponent<C0Patches>(entity), u, v); }

    alg::Vec3 PartialDerivativeU(const C0Patches& patches, float u, float v) const;
    alg::Vec3 PartialDerivativeU(const Entity entity, const float u, const float v) const override
        { return PartialDerivativeU(coordinator->GetComponent<C0Patches>(entity), u, v); }

    alg::Vec3 PartialDerivativeV(const C0Patches& patches, float u, float v) const;
    alg::Vec3 PartialDerivativeV(const Entity entity, const float u, const float v) const override
        { return PartialDerivativeV(coordinator->GetComponent<C0Patches>(entity), u, v); }

    static float MaxU(const C0Patches& patches)
        { return static_cast<float>(patches.PatchesInRow()); }

    float MaxU(const Entity entity) const override
        { return MaxU(coordinator->GetComponent<C0Patches>(entity)); }

    static float MaxV(const C0Patches& patches)
        { return static_cast<float>(patches.PatchesInCol()); }

    float MaxV(const Entity entity) const override
        { return MaxV(coordinator->GetComponent<C0Patches>(entity)); }

    void Render(const alg::Mat4x4& cameraMtx) const;

    void Update() const;

private:
    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;

    void UpdateMesh(Entity surface, const C0Patches& patches) const;

    static void CheckUVDomain(const C0Patches& patches, float u, float v);
    static void NormalizeUV(const C0Patches& patches, float& u, float& v);

    static bool ShouldWrapU(const C0Patches& patches);
    static bool ShouldWrapV(const C0Patches& patches);

    std::vector<float> GenerateVertices(const C0Patches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C0Patches& patches) const;

    class DeletionHandler final : public EventHandler<C0Patches> {
    public:
        explicit DeletionHandler(Coordinator& coordinator):
            coordinator(coordinator) {}

        void HandleEvent(Entity entity, const C0Patches& component, EventType eventType) override;

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
