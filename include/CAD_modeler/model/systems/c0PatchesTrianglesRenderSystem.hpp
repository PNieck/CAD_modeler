#pragma once

#include <ecs/system.hpp>

#include <algebra/mat4x4.hpp>

#include "../components/c0Patches.hpp"

#include "shaders/bicubicBezierSurfaceTrianglesShader.hpp"


class C0PatchesTrianglesRenderSystem : public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void AddSurface(Entity entity, float densityLevel = BicubicBezierSurfaceTrianglesShader::MaxTessellationLevel);
    void RemoveSurface(Entity entity);

    void Render(const alg::Mat4x4& cameraMtx);
private:

    BicubicBezierSurfaceTrianglesShader shader;

    std::vector<float> GenerateVertices(const C0Patches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C0Patches& patches) const;
};
