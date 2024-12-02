#include <CAD_modeler/model/systems/positionSystem.hpp>

#include <ecs/coordinator.hpp>

#include <algebra/quat.hpp>


void PositionSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<PositionSystem>();
}


float PositionSystem::Distance(Entity e, Position p) const
{
    auto const& pos = coordinator->GetComponent<Position>(e);

    return alg::Distance(pos.vec, p.vec);
}


void PositionSystem::RotateAround(const Entity target, const Position &pivot, const float x, const float y)
{
    auto const& position = coordinator->GetComponent<Position>(target);
    auto posSub = position.vec - pivot.vec;

    alg::Quat q1(alg::Vec3(0.f, 1.f, 0.f), x);

    const auto axis = alg::Cross(posSub, alg::Vec3(0.f, 1.f, 0.f));
    q1 = q1 * alg::Quat(axis, y);

    posSub = q1.Rotate(posSub);
    auto newPos = posSub + pivot.vec;

    coordinator->SetComponent<Position>(target, Position(newPos.X(), newPos.Y(), newPos.Z()));
}


void PositionSystem::SetDistance(float newDist, Entity target, Position pivot)
{
    Position const& position = coordinator->GetComponent<Position>(target);

    Position newPos((position.vec - pivot.vec).Normalize() * newDist + pivot.vec);
    coordinator->SetComponent<Position>(target, newPos);
}
