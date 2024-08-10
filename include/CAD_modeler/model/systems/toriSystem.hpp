#pragma once

#include <ecs/system.hpp>
#include "utils/nameGenerator.hpp"
#include "shaders/shaderRepository.hpp"
#include "../components/position.hpp"
#include "../components/torusParameters.hpp"


class ToriSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(ShaderRepository* shaderRepo);

    Entity AddTorus(const Position& pos, const TorusParameters& params);

    void SetTorusParameter(Entity entity, const TorusParameters& params);

    void Render(const alg::Mat4x4& cameraMtx);

private:
    NameGenerator nameGenerator;

    ShaderRepository* shadersRepo;
};
