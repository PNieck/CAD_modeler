#pragma once

#include "../components/c0Patches.hpp"
#include "../components/gregoryPatchParameters.hpp"

#include "shaders/shaderRepository.hpp"

#include <ecs/system.hpp>

#include <algebra/mat4x4.hpp>

#include <vector>


class GregoryPatchesSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(ShaderRepository* shadersRepo);

    class Hole {
    public:
        static const int innerCpNb = 9;
        static const int outerCpNb = 12;

        inline Entity GetInnerControlPoint(int index) const
            { return innerCp[index]; }

        inline Entity GetOuterControlPoint(int index) const
            { return outerCp[index]; }

    private:
        Entity innerCp[innerCpNb];
        Entity outerCp[outerCpNb];

        friend class GregoryPatchesSystem;
    };

    std::vector<Hole> FindHolesToFill(const std::vector<C0Patches>& patches) const;

    Entity FillHole(const Hole& hole);

    void ShowControlNet(Entity gregoryPatches);
    void HideControlNet(Entity gregoryPatches);

    void Render(const alg::Mat4x4& cameraMtx) const;

private:
    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;

    ShaderRepository* shaderRepo;

    void UpdateEntities() const;
    void UpdateMesh(Entity entity, const TriangleOfGregoryPatches& triangle) const;
    void FillPatchesParameters(TriangleOfGregoryPatches& triangle, Entity entity) const;

    void RenderNet(std::stack<Entity>& entities, const alg::Mat4x4& cameraMtx) const;

    std::vector<float> GenerateGregoryPatchVertices(const TriangleOfGregoryPatches& params) const;
    std::vector<uint32_t> GenerateGregoryPatchIndices(const TriangleOfGregoryPatches& params) const;

    std::vector<float> GenerateNetVertices(const TriangleOfGregoryPatches& params) const;
    std::vector<uint32_t> GenerateNetIndices() const;

    class DeletionHandler: public EventHandler<TriangleOfGregoryPatches> {
    public:
        DeletionHandler(Coordinator& coordinator):
            coordinator(coordinator) {}

        void HandleEvent(Entity entity, const TriangleOfGregoryPatches& component, EventType eventType) override;

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
