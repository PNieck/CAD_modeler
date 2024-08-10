#include <CAD_modeler/model/systems/cameraSystem.hpp>

#include <algebra/vec4.hpp>

#include <CAD_modeler/model/components/cameraParameters.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/rotation.hpp>

#include <CAD_modeler/utilities/angle.hpp>


void CameraSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<CameraSystem>();
}


void CameraSystem::Init(const CameraParameters& params, const Position& cameraPos)
{
    camera = coordinator->CreateEntity();

    coordinator->AddComponent<Position>(camera, cameraPos);
    coordinator->AddComponent<CameraParameters>(camera, params);
}


alg::Mat4x4 CameraSystem::ViewMatrix() const
{
    Position const& position = coordinator->GetComponent<Position>(camera);
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);

    alg::Vec3 direction = (position.vec - params.target.vec).Normalize();
    
    return alg::LookAt(position.vec, direction, globalUp);
}


alg::Mat4x4 CameraSystem::PerspectiveMatrix() const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);

    float aspectRatio = params.GetAspectRatio();

    float v1 = 1.0f/std::tan(params.fov/2.0);
    float v2 = v1/aspectRatio;
    float v3 = (params.farPlane + params.nearPlane)/(params.farPlane - params.nearPlane);
    float v4 = -2.0 * (params.farPlane * params.nearPlane) / (params.farPlane - params.nearPlane);

    alg::Mat4x4 result(
        v2, 0.0f, 0.0f, 0.0f,
        0.0f, v1, 0.0f, 0.0f,
        0.0f, 0.0f, -v3, v4,
        0.0f, 0.0f, -1.0f, 0.0f
    );

    result.TransposeSelf();

    return result;
}
