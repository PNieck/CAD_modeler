#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "../components/position.hpp"
#include "../components/cameraParameters.hpp"

#include <algebra/mat4x4.hpp>


class CameraSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(const CameraParameters& params, const Position& cameraPos);

    alg::Mat4x4 ViewMatrix() const;
    alg::Mat4x4 PerspectiveMatrix() const;


    inline Position GetCameraPos() const
        { return coordinator->GetComponent<Position>(camera); }

    inline void SetCameraPos(const Position& pos)
        { coordinator->SetComponent<Position>(camera, pos); }

    inline CameraParameters GetParameters() const
        { return coordinator->GetComponent<CameraParameters>(camera); }

    inline void SetParameters(const CameraParameters& params)
        { coordinator->SetComponent<CameraParameters>(camera, params); }

    inline Entity GetCameraEntity() const
        { return camera; }

private:
    static constexpr alg::Vec3 globalUp = alg::Vec3(0.0f, 1.0f, 0.0f);

    Entity camera;
};
