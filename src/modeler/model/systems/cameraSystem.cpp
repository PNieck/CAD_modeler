#include <CAD_modeler/model/systems/cameraSystem.hpp>

#include <CAD_modeler/model/components/cameraParameters.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/rotation.hpp>


void CameraSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<CameraSystem>();

    coordinator.RegisterComponent<Position>();
    coordinator.RegisterComponent<CameraParameters>();
}


void CameraSystem::Init(const CameraParameters& params, const Position& cameraPos)
{
    camera = coordinator->CreateEntity();

    coordinator->AddComponent<Position>(camera, cameraPos);
    coordinator->AddComponent<CameraParameters>(camera, params);
}


alg::Mat4x4 CameraSystem::ViewMatrix() const
{
    auto const& position = coordinator->GetComponent<Position>(camera);
    auto const& params = coordinator->GetComponent<CameraParameters>(camera);

    const alg::Vec3 direction = (position.vec - params.target.vec).Normalize();
    
    return LookAt(position.vec, direction, globalUp);
}


alg::Mat4x4 CameraSystem::PerspectiveMatrix() const
{
    auto const& params = coordinator->GetComponent<CameraParameters>(camera);

    const float aspectRatio = params.GetAspectRatio();

    const float v1 = 1.0f/std::tan(params.fov/2.0f);
    const float v2 = v1/aspectRatio;
    const float v3 = (params.farPlane + params.nearPlane)/(params.farPlane - params.nearPlane);
    const float v4 = -2.f * (params.farPlane * params.nearPlane) / (params.farPlane - params.nearPlane);

    alg::Mat4x4 result(
        v2, 0.0f, 0.0f, 0.0f,
        0.0f, v1, 0.0f, 0.0f,
        0.0f, 0.0f, -v3, v4,
        0.0f, 0.0f, -1.0f, 0.0f
    );

    result.TransposeSelf();

    return result;
}
