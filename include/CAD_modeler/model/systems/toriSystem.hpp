#pragma once

#include "system.hpp"
#include "utils/nameGenerator.hpp"
#include "../components/position.hpp"
#include "../components/torusParameters.hpp"


class ToriSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity AddTorus(const Position& pos, const TorusParameters& params);

    void SetTorusParameter(Entity entity, const TorusParameters& params);

private:
    NameGenerator nameGenerator;
};
