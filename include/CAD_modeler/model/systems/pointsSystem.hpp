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

    inline void Init(ShaderRepository* shaderRepo)
        { this->shaderRepo = shaderRepo; }


    Entity CreatePoint(const Position& pos);

    void Render() const;

private:
    Mesh pointsMesh;
    ShaderRepository* shaderRepo;

    NameGenerator nameGenerator;
};
