#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "../components/position.hpp"


class PositionSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    [[nodiscard]]
    float Distance(const Entity e1, const Entity e2) const
        { return Distance(e1, coordinator->GetComponent<Position>(e2)); }

    [[nodiscard]]
    float Distance(Entity e, Position p) const;

    inline void RotateAround(Entity target, Entity pivot, float x, float y)
        { RotateAround(target, coordinator->GetComponent<Position>(pivot), x, y); }

    void RotateAround(Entity target, const Position& pivot, float x, float y);

    inline void SetDistance(float newDist, Entity target, Entity pivot)
        { SetDistance(newDist, target, coordinator->GetComponent<Position>(pivot)); }

    void SetDistance(float newDist, Entity target, Position pivot);
};
