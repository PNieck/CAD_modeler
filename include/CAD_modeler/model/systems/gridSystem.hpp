#pragma once


#include <ecs/system.hpp>
#include "../../shader.hpp"


class GridSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    GridSystem();

    void Init();

    void Render() const;

private:
    Shader gridShader;
    Entity grid;
};
