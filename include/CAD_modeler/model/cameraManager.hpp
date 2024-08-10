#pragma once

#include <ecs/coordinator.hpp>

#include <algebra/mat4x4.hpp>

#include <variant>

#include "systems/cameraSystem.hpp"
#include "systems/anaglyphsCameraSystem.hpp"


class CameraManager {
public:
    enum class CameraType {
        Perspective,
        Anaglyphs
    };

    using Eye = AnaglyphsCameraSystem::Eye;

    CameraManager(Coordinator& coordinator);

    void Init(int viewportWidth, int viewportHeight);

    alg::Mat4x4 PerspectiveMtx() const;
    alg::Mat4x4 ViewMtx() const;

    void RotateCamera(float x, float y);

    float GetDistanceFromTarget() const;
    void SetDistanceFromTarget(float newDist);

    CameraType GetCurrentCameraType() const;
    
    void SetCameraType(CameraType camType);

    // Works only when anaglyph camera type is selected
    void SetCurrentEye(Eye eye);

    inline float GetEyeSeparation() const
        { return eyeSeparation; }

    inline float GetConvergence() const
        { return convergence; }

    void SetEyeSeparation(float eyeSeparation);
    void SetConvergence(float convergence);

    CameraParameters GetBaseParams() const;
    void SetBaseParams(const CameraParameters& params);

    inline Position GetCameraPosition() const
        { return coordinator.GetComponent<Position>(GetCameraEntity()); }

private:
    Coordinator& coordinator;

    using PerspectiveSys = std::shared_ptr<CameraSystem>;
    using AnaglyphsSys = std::shared_ptr<AnaglyphsCameraSystem>;

    std::variant<PerspectiveSys, AnaglyphsSys> currentCameraSys;

    float eyeSeparation = 1.0f;
    float convergence = 20.f;

    void SwitchToPerspectiveCameraType();

    void SwitchToAnaglyphsCameraType();

    Entity GetCameraEntity() const;
};
