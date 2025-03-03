#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "../components/position.hpp"

#include <algebra/vec3.hpp>


class CursorSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    void Render(const alg::Mat4x4& cameraMtx);

    void SetPosition(const alg::Vec3& position)
        { coordinator->SetComponent<Position>(cursor, Position(position)); }

    Position GetPosition() const
        { return coordinator->GetComponent<Position>(cursor); }

    Entity GetCursor() const
        { return cursor; }

private:
    Entity cursor;
};
