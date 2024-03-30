#pragma once


#include "system.hpp"
#include "../../shader.hpp"


class CursorSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    CursorSystem();

    void Init();

    void Render();

private:
    Entity cursor;
    Shader shader;
};
