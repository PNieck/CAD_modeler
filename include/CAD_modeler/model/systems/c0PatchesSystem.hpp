#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "../components/c0Patches.hpp"
#include "../components/position.hpp"
#include "controlNetSystem.hpp"


class C0PatchesSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline int GetRowsCnt(Entity surface) const
        { return coordinator->GetComponent<C0Patches>(surface).PatchesInRow(); }

    inline int GetColsCnt(Entity surface) const
        { return coordinator->GetComponent<C0Patches>(surface).PatchesInCol(); }

    void MergeControlPoints(Entity patches, Entity oldCP, Entity newCP, SystemId system);

    void ShowBezierNet(Entity surface);

    inline void HideBezierNet(Entity surface)
        { coordinator->GetSystem<ControlNetSystem>()->DeleteControlPointsNet(surface); }

    Position PointOnPatches(Entity entity, float u, float v) const;

    void Render(const alg::Mat4x4& cameraMtx) const;

private:
    void UpdateEntities() const;
    void UpdateMesh(Entity surface, const C0Patches& patches) const;

    std::vector<float> GenerateVertices(const C0Patches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C0Patches& patches) const;
};
