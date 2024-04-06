#pragma once


#include <ecs/system.hpp>
#include "../../shader.hpp"
#include "../components/position.hpp"

#include <glm/vec3.hpp>


class CursorSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    CursorSystem();

    void Init();

    void Render();

    void SetPosition(const glm::vec3& position);

    Position GetPosition() const;

private:
    Entity cursor;
    Shader shader;
};
