#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "utils/nameGenerator.hpp"
#include "../components/position.hpp"
#include "../components/controlPoints.hpp"


class C0SurfaceSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    Entity CreateSurface(const Position& pos);

    void Render() const;

private:
    ShaderRepository* shaderRepo;
    NameGenerator nameGenerator;

    void UpdateEntities() const;
    void UpdateMesh(Entity surface) const;

    std::vector<float> GenerateVertices(const ControlPoints& cps) const;
    std::vector<uint32_t> GenerateIndices(const ControlPoints& cps) const;
};
