#pragma once

#include <ecs/system.hpp>

#include <memory>
#include "../components/curveControlPoints.hpp"


class CurveControlPointsSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    Entity CreateControlPoints(const std::vector<Entity>& entities);

    void AddControlPoint(Entity curve, Entity entity);

    void DeleteControlPoint(Entity curve, Entity entity);

private:
    std::shared_ptr<DeletionHandler> deletionHandler;
};
