#pragma once

#include <ecs/system.hpp>


class ToUpdateSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);
};
