#pragma once

#include <ecs/system.hpp>
#include "../components/position.hpp"
#include "shaders/shaderRepository.hpp"

#include <algebra/vec3.hpp>


class CursorSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(ShaderRepository* shadersRepo);

    void Render(const alg::Mat4x4& cameraMtx);

    void SetPosition(const alg::Vec3& position);

    Position GetPosition() const;

private:
    Entity cursor;
    ShaderRepository* shaderRepo;
};
