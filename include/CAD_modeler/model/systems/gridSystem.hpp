#pragma once

#include <ecs/system.hpp>
#include <algebra/mat4x4.hpp>


class GridSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    void Render(const alg::Mat4x4& viewMtx, const alg::Mat4x4& projMtx, float nearPlane, float farPlane) const;

private:
    Entity grid = 0;
};
