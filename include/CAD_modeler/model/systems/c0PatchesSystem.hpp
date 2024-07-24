#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/c0Patches.hpp"
#include "../components/patchesPolygonMesh.hpp"
#include "../components/hasPatchesPolygon.hpp"


class C0PatchesSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    inline int GetRowsCnt(Entity surface) const
        { return coordinator->GetComponent<C0Patches>(surface).PatchesInRow(); }

    inline int GetColsCnt(Entity surface) const
        { return coordinator->GetComponent<C0Patches>(surface).PatchesInCol(); }

    inline void AddPossibilityToHasPatchesPolygon(Entity entity) const
        { coordinator->AddComponent<HasPatchesPolygon>(entity, HasPatchesPolygon()); }

    void ShowPolygon(Entity entity) const;
    void HidePolygon(Entity entity) const;

    void Render() const;

private:
    ShaderRepository* shaderRepo;

    inline bool HasPolygon(Entity entity) const
        { return coordinator->GetEntityComponents(entity).contains(coordinator->GetComponentID<PatchesPolygonMesh>()); }

    void UpdateEntities() const;
    void UpdateMesh(Entity surface) const;
    void UpdatePolygonMesh(Entity entity) const;

    void RenderPolygon(std::stack<Entity>& entities) const;

    std::vector<float> GenerateVertices(const C0Patches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C0Patches& patches) const;

    std::vector<float> GeneratePolygonVertices(const C0Patches& patches) const;
    std::vector<uint32_t> GeneratePolygonIndices(const C0Patches& patches) const;
};
