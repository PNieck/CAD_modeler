#pragma once

#include <ecs/system.hpp>
#include "../components/position.hpp"
#include "../components/mesh.hpp"


class PointsSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    PointsSystem();

    Entity CreatePoint(const Position& pos = Position());

    void Render(const alg::Mat4x4& cameraMtx) const;

private:
    Mesh pointsMesh;
};
