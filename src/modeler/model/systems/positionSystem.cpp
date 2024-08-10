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

void PositionSystem::RotateAround(Entity target, const Position &pivot, float x, float y, float z)
{
    Position const& position = coordinator->GetComponent<Position>(target);

    alg::Mat4x4 translationMtx = pivot.TranslationMatrix();
    alg::Mat4x4 translationInv = translationMtx.Inverse().value();

    alg::Quat q(x, y, z);
    alg::Vec4 pos(position.vec, 1.0f);

    pos = translationMtx * q.ToRotationMatrix() * translationInv * pos;

    coordinator->SetComponent<Position>(target, Position(pos.X(), pos.Y(), pos.Z()));
}


void PositionSystem::SetDistance(float newDist, Entity target, Position pivot)
{
    Position const& position = coordinator->GetComponent<Position>(target);

    Position newPos((pivot.vec - position.vec).Normalize() * newDist + pivot.vec);
    coordinator->SetComponent<Position>(target, newPos);
}
