#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "../../components/position.hpp"
#include "../../components/cameraParameters.hpp"

#include <algebra/mat4x4.hpp>


class PerspectiveCameraSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(const CameraParameters& params, const Position& cameraPos);

    [[nodiscard]]
    alg::Mat4x4 ViewMatrix() const;

    [[nodiscard]]
    alg::Mat4x4 PerspectiveMatrix() const;


    [[nodiscard]]
    Position GetCameraPos() const
        { return coordinator->GetComponent<Position>(camera); }

    void SetCameraPos(const Position& pos) const
        { coordinator->SetComponent<Position>(camera, pos); }

    [[nodiscard]]
    CameraParameters GetParameters() const
        { return coordinator->GetComponent<CameraParameters>(camera); }

    void SetParameters(const CameraParameters& params) const
        { coordinator->SetComponent<CameraParameters>(camera, params); }

    [[nodiscard]]
    Entity GetCameraEntity() const
        { return camera; }

private:
    static constexpr auto globalUp = alg::Vec3(0.0f, 1.0f, 0.0f);

    Entity camera = 0;
};
