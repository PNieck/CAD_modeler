#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/c0SurfacePatches.hpp"


class C0PatchesSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    void Render() const;

private:
    ShaderRepository* shaderRepo;

    void UpdateEntities() const;
    void UpdateMesh(Entity surface) const;

    std::vector<float> GenerateVertices(const C0SurfacePatches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C0SurfacePatches& patches) const;
};
