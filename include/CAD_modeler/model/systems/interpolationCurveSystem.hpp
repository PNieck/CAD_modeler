#pragma once

#include <ecs/system.hpp>

#include "curveControlPointsSystem.hpp"

#include <vector>


class InterpolationCurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity CreateCurve(const std::vector<Entity>& controlPoints);

    void AddControlPoint(Entity curve, Entity cp) const;

    void DeleteControlPoint(Entity curve, Entity cp) const;

    void MergeControlPoints(Entity curve, Entity oldCP, Entity newCP) const;
};
 