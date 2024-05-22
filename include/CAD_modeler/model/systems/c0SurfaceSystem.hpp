#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "utils/nameGenerator.hpp"
#include "../components/position.hpp"
#include "../components/controlPoints.hpp"
#include "../components/c0SurfacePatches.hpp"


class C0SurfaceSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    Entity CreateSurface(const Position& pos);

    void AddRowOfPatches(Entity surface) const;
    void AddColOfPatches(Entity surface) const;

    void DeleteRowOfPatches(Entity surface) const;
    void DeleteColOfPatches(Entity surface) const;

    void Render() const;

private:
    ShaderRepository* shaderRepo;
    NameGenerator nameGenerator;

    static const alg::Vec3 offsetX;
    static const alg::Vec3 offsetZ;

    void UpdateEntities() const;
    void UpdateMesh(Entity surface) const;

    std::vector<float> GenerateVertices(const C0SurfacePatches& patches) const;
    std::vector<uint32_t> GenerateIndices(const C0SurfacePatches& patches) const;
};
