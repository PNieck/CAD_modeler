#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>
#include "../components/position.hpp"
#include "../components/anaglypsCameraParameters.hpp"



class AnaglyphsCameraSystem: public System {
public:
    enum class Eye {
        Left,
        Right,
    };

    static void RegisterSystem(Coordinator& coordinator);

    void Init(const AnaglyphsCameraParameters& params, const Position& cameraPos);

    alg::Mat4x4 ViewMatrix() const;
    alg::Mat4x4 PerspectiveMatrix() const;

    inline void SetCurrentEye(Eye eye)
        { currentEye = eye; }

    inline Eye GetCurrentEye() const
        { return currentEye; }

    inline Position GetCameraPos() const
        { return coordinator->GetComponent<Position>(camera); }

    inline void SetCameraPos(const Position& pos)
        { coordinator->SetComponent<Position>(camera, pos); }

    inline AnaglyphsCameraParameters GetParameters() const
        { return coordinator->GetComponent<AnaglyphsCameraParameters>(camera); }

    inline void SetParameters(const AnaglyphsCameraParameters& params)
        { coordinator->SetComponent<AnaglyphsCameraParameters>(camera, params); }

    inline Entity GetCameraEntity() const
        { return camera; }

private:
    static constexpr alg::Vec3 globalUp = alg::Vec3(0.0f, 1.0f, 0.0f);

    Entity camera;
    Eye currentEye;
};
