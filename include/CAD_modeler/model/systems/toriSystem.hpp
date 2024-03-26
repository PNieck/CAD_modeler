#pragma once

#include "system.hpp"


class ToriSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity AddTorus();
};
