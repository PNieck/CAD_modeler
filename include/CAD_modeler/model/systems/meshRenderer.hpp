#pragma once

#include "system.hpp"
#include "../../shader.hpp"


class MeshRenderer: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    MeshRenderer();

    void Render();

private:
    Shader shader;
};
