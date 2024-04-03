#pragma once


#include "system.hpp"
#include "utils/nameGenerator.hpp"
#include "../components/position.hpp"
#include "../components/mesh.hpp"
#include "../../shader.hpp"


class PointsSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    PointsSystem();

    Entity CreatePoint(Position& pos);

    void Render() const;

private:
    Mesh pointsMesh;
    Shader shader;

    NameGenerator nameGenerator;
};
