#pragma once

#include <ecs/system.hpp>
#include "shaders/shaderRepository.hpp"


class GridSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(ShaderRepository* shaderRepo);

    void Render(const alg::Mat4x4& viewMtx, const alg::Mat4x4& projMtx, float nearPlane, float farPlane) const;

private:
    Entity grid;

    ShaderRepository* shaderRepo;
};
