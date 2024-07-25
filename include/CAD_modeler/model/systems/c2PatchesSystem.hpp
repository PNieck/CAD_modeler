#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/c2Patches.hpp"


class C2PatchesSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    inline int GetRowsCnt(Entity surface) const
        { return coordinator->GetComponent<C2Patches>(surface).PatchesInRow(); }

    inline int GetColsCnt(Entity surface) const
        { return coordinator->GetComponent<C2Patches>(surface).PatchesInCol(); }

    void Render() const;

private:
    ShaderRepository* shaderRepo;

    void UpdateEntities() const;
    void UpdateMesh(Entity surface) const;

    std::vector<float> GenerateVertices(const C2Patches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C2Patches& patches) const;
};
