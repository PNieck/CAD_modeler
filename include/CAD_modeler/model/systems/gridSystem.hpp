#pragma once

#include <ecs/system.hpp>
#include "shaders/shaderRepository.hpp"


class GridSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(ShaderRepository* shaderRepo);

    void Render() const;

private:
    Entity grid;

    ShaderRepository* shaderRepo;
};
