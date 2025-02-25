#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "../components/position.hpp"
#include "../components/patchesDensity.hpp"
#include "../components/c2CylinderPatches.hpp"
#include "controlNetSystem.hpp"


class C2CylinderSystem: public System {
public:
    static constexpr int DoublePointsCnt = 3;

    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    Entity CreateCylinder(const Position& pos, const alg::Vec3& direction, float radius);
    
    void AddRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;
    void AddColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;

    void DeleteRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;
    void DeleteColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;

    void MergeControlPoints(Entity cylinder, Entity oldCP, Entity newCP);

    inline void SetDensity(Entity entity, PatchesDensity density) const
        { coordinator->SetComponent<PatchesDensity>(entity, density); }

    inline int GetRowsCnt(Entity cylinder) const
        { return coordinator->GetComponent<C2CylinderPatches>(cylinder).PatchesInRow(); }

    inline int GetColsCnt(Entity cylinder) const
        { return coordinator->GetComponent<C2CylinderPatches>(cylinder).PatchesInCol(); }

    void ShowDeBoorNet(Entity cylinder);

    inline void HideDeBoorNet(Entity cylinder)
        { coordinator->GetSystem<ControlNetSystem>()->DeleteControlPointsNet(cylinder); }

    void Recalculate(Entity cylinder, const Position& pos, const alg::Vec3& direction, float radius) const;

    void Render(const alg::Mat4x4& cameraMtx) const;

private:
    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;

    void UpdateEntities() const;
    void UpdateMesh(Entity surface, const C2CylinderPatches& patches) const;

    void UpdateDoubleControlPoints(C2CylinderPatches& patches) const;

    std::vector<float> GenerateVertices(const C2Patches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C2Patches& patches) const;

    class DeletionHandler: public EventHandler<C2CylinderPatches> {
    public:
        DeletionHandler(Coordinator& coordinator):
            coordinator(coordinator) {}

        void HandleEvent(Entity entity, const C2CylinderPatches& component, EventType eventType) override;

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
