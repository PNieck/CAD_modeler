#pragma once

#include <ecs/coordinator.hpp>

#include <variant>

#include "../systems/camera/perspectiveCameraSystem.hpp"
#include "../systems/camera/anaglyphsCameraSystem.hpp"


class CameraManager {
public:
    enum class CameraType {
        Perspective,
        Anaglyphs
    };

    using Eye = AnaglyphsCameraSystem::Eye;

    explicit CameraManager(Coordinator& coordinator);

    void Init(int viewportWidth, int viewportHeight);

    [[nodiscard]]
    alg::Mat4x4 PerspectiveMtx() const;

    [[nodiscard]]
    alg::Mat4x4 ViewMtx() const;

    void RotateCamera(float x, float y);

    [[nodiscard]]
    float GetDistanceFromTarget() const;

    void SetDistanceFromTarget(float newDist);

    [[nodiscard]]
    CameraType GetCurrentCameraType() const;
    
    void SetCameraType(CameraType camType);

    // Works only when anaglyphs camera type is selected
    void SetCurrentEye(Eye eye);

    [[nodiscard]]
    float GetEyeSeparation() const
        { return eyeSeparation; }

    [[nodiscard]]
    float GetConvergence() const
        { return convergence; }

    void SetEyeSeparation(float eyeSeparation);
    void SetConvergence(float convergence);

    [[nodiscard]]
    CameraParameters GetBaseParams() const;

    void SetBaseParams(const CameraParameters& params);

    [[nodiscard]]
    Position GetCameraPosition() const
        { return coordinator.GetComponent<Position>(GetCameraEntity()); }

private:
    Coordinator& coordinator;

    using PerspectiveSys = std::shared_ptr<PerspectiveCameraSystem>;
    using AnaglyphsSys = std::shared_ptr<AnaglyphsCameraSystem>;

    std::variant<PerspectiveSys, AnaglyphsSys> currentCameraSys;

    float eyeSeparation = 1.0f;
    float convergence = 20.f;

    void SwitchToPerspectiveCameraType();

    void SwitchToAnaglyphsCameraType();

    [[nodiscard]]
    Entity GetCameraEntity() const;
};
