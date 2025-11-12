#pragma once

#include <ecs/system.hpp>

#include <algebra/mat4x4.hpp>

#include "../components/c2Patches.hpp"

#include "shaders/bicubicBSplineSurfaceTrianglesShader.hpp"


class C2PatchesTrianglesRenderSystem : public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void AddSurface(Entity entity, float densityLevel = BicubicBSplineSurfaceTrianglesShader::MaxTessellationLevel);
    void RemoveSurface(Entity entity);

    void Render(const alg::Mat4x4& cameraMtx);
private:

    BicubicBSplineSurfaceTrianglesShader shader;

    std::vector<float> GenerateVertices(const C2Patches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C2Patches& patches) const;
};
