#pragma once

#include <ecs/system.hpp>

#include "utils/nameGenerator.hpp"
#include "../components/position.hpp"
#include "../components/c0PatchesDensity.hpp"
#include "../components/c2CylinderPatches.hpp"
#include "c2PatchesSystem.hpp"


class C2CylinderSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(ShaderRepository* shadersRepo);

    Entity CreateCylinder(const Position& pos, const alg::Vec3& direction, float radius);
    
    void AddRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;
    void AddColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;

    void DeleteRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;
    void DeleteColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;

    inline void SetDensity(Entity entity, C0PatchesDensity density) const
        { coordinator->SetComponent<C0PatchesDensity>(entity, density); }

    inline int GetRowsCnt(Entity cylinder) const
        { return coordinator->GetComponent<C2CylinderPatches>(cylinder).PatchesInRow(); }

    inline int GetColsCnt(Entity cylinder) const
        { return coordinator->GetComponent<C2CylinderPatches>(cylinder).PatchesInCol(); }

    void Recalculate(Entity cylinder, const Position& pos, const alg::Vec3& direction, float radius) const;

    void Render() const;

private:
    static constexpr int doublePointsCnt = 3;

    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;
    NameGenerator nameGenerator;

    ShaderRepository* shaderRepo;

    void UpdateEntities() const;
    void UpdateMesh(Entity surface) const;

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
