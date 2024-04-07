#pragma once

#include <ecs/system.hpp>
#include "../components/position.hpp"
#include "shaders/shaderRepository.hpp"

#include <glm/vec3.hpp>


class CursorSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(ShaderRepository* shadersRepo);

    void Render();

    void SetPosition(const glm::vec3& position);

    Position GetPosition() const;

private:
    Entity cursor;
    ShaderRepository* shaderRepo;
};
