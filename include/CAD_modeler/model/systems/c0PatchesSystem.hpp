#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "../components/c0Patches.hpp"
#include "../components/position.hpp"
#include "controlNetSystem.hpp"


class C0PatchesSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    int GetRowsCnt(const Entity surface) const
        { return coordinator->GetComponent<C0Patches>(surface).PatchesInRow(); }

    int GetColsCnt(const Entity surface) const
        { return coordinator->GetComponent<C0Patches>(surface).PatchesInCol(); }

    void MergeControlPoints(Entity patches, Entity oldCP, Entity newCP, SystemId system);

    void ShowBezierNet(Entity surface);

    void HideBezierNet(const Entity surface) const
        { coordinator->GetSystem<ControlNetSystem>()->DeleteControlPointsNet(surface); }

    Position PointOnPatches(const C0Patches& patches, float u, float v) const;
    Position PointOnPatches(const Entity entity, const float u, const float v) const
        { return PointOnPatches(coordinator->GetComponent<C0Patches>(entity), u, v); }

    alg::Vec3 PartialDerivativeU(const C0Patches& patches, float u, float v) const;
    alg::Vec3 PartialDerivativeU(const Entity entity, const float u, const float v) const
        { return PartialDerivativeU(coordinator->GetComponent<C0Patches>(entity), u, v); }

    alg::Vec3 PartialDerivativeV(const C0Patches& patches, float u, float v) const;
    alg::Vec3 PartialDerivativeV(const Entity entity, const float u, const float v) const
        { return PartialDerivativeV(coordinator->GetComponent<C0Patches>(entity), u, v); }

    static float MaxU(const C0Patches& patches)
        { return static_cast<float>(patches.PatchesInRow()); }

    float MaxU(const Entity entity) const
        { return MaxU(coordinator->GetComponent<C0Patches>(entity)); }

    static float MaxV(const C0Patches& patches)
        { return static_cast<float>(patches.PatchesInCol()); }

    float MaxV(const Entity entity) const
        { return MaxV(coordinator->GetComponent<C0Patches>(entity)); }

    void Render(const alg::Mat4x4& cameraMtx) const;

    void Update() const;

private:
    void UpdateMesh(Entity surface, const C0Patches& patches) const;

    std::vector<float> GenerateVertices(const C0Patches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C0Patches& patches) const;
};
