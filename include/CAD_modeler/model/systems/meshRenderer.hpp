#pragma once

#include <ecs/system.hpp>
#include "shaders/shaderRepository.hpp"


class MeshRenderer: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    MeshRenderer();

    void Render();

private:
    Shader shader;
};
