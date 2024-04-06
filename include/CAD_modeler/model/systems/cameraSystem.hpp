#pragma once

#include <ecs/system.hpp>
#include "../components/position.hpp"

#include <glm/mat4x4.hpp>


class CameraSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(int viewport_width, int viewport_height);

    glm::mat4x4 ViewMatrix() const;
    glm::mat4x4 PerspectiveMatrix() const;

    void RotateAroundTarget(float x, float y) const;

    Position GetPosition() const;

    Position GetTargetPosition() const;

    float GetDistanceToTarget() const;

    void SetDistanceToTarget(float newDist) const;

    void MultiplyDistanceToTarget(float coefficient) const;

    float GetNearPlane() const;

    float GetFarPlane() const;

    void ChangeViewportSize(int width, int height) const;

private:
    static constexpr glm::vec3 globalUp = glm::vec3(0.0f, 1.0f, 0.0f);

    Entity camera;
};
