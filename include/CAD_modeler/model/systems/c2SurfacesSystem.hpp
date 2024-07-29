#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "shaders/shaderRepository.hpp"
#include "utils/nameGenerator.hpp"
#include "../components/position.hpp"
#include "../components/c2Patches.hpp"
#include "../components/patchesDensity.hpp"


class C2SurfaceSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(ShaderRepository* shadersRepo);

    Entity CreateSurface(const Position& pos, const alg::Vec3& direction, float length, float width);

    void AddRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;
    void AddColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

    void DeleteRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;
    void DeleteColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

    inline void SetDensity(Entity entity, PatchesDensity density) const
        { coordinator->SetComponent<PatchesDensity>(entity, density); }

    inline int GetRowsCnt(Entity surface) const
        { return coordinator->GetComponent<C2Patches>(surface).PatchesInRow(); }

    inline int GetColsCnt(Entity surface) const
        { return coordinator->GetComponent<C2Patches>(surface).PatchesInCol(); }

    // inline void ShowBezierPolygon(Entity cylinder) const
    //     { coordinator->GetSystem<C2PatchesSystem>()->ShowPolygon(cylinder); }

    // inline void HideBezierPolygon(Entity cylinder) const
    //     { coordinator->GetSystem<C2PatchesSystem>()->HidePolygon(cylinder); }

    void Recalculate(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

    void Render() const;

private:
    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;
    NameGenerator nameGenerator;
    ShaderRepository* shaderRepo;

    void UpdateEntities() const;
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
