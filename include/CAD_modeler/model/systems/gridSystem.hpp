#pragma once


#include "system.hpp"
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
