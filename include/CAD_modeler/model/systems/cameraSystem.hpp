#pragma once

#include <ecs/system.hpp>
#include "../components/position.hpp"
#include "../components/cameraParameters.hpp"

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>


class CameraSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(const CameraParameters& params, const Position& cameraPos);

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

    int GetViewportWidth() const;

    int GetViewportHeight() const;

    float GetFov() const;

    void ChangeViewportSize(int width, int height) const;

    inline Entity GetCameraEntity() const
        { return camera; }

private:
    static constexpr glm::vec3 globalUp = glm::vec3(0.0f, 1.0f, 0.0f);

    Entity camera;
};
