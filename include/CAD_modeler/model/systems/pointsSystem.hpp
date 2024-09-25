#pragma once

#include <ecs/system.hpp>
#include "utils/nameGenerator.hpp"
#include "../components/position.hpp"
#include "../components/mesh.hpp"
#include "shaders/shaderRepository.hpp"


class PointsSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    PointsSystem();

    inline void Init(ShaderRepository* shaders)
        { this->shaderRepo = shaders; }

    Entity CreatePoint(const Position& pos = Position(), bool createName = true);

    void Render(const alg::Mat4x4& cameraMtx) const;

private:
    Mesh pointsMesh;
    ShaderRepository* shaderRepo;

    NameGenerator nameGenerator;
};
