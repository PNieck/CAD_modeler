#include <CAD_modeler/model/systems/camera/anaglyphsCameraSystem.hpp>

#include <stdexcept>


void AnaglyphsCameraSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<AnaglyphsCameraSystem>();

    coordinator.RegisterComponent<Position>();
    coordinator.RegisterComponent<AnaglyphsCameraParameters>();
}


void AnaglyphsCameraSystem::Init(const AnaglyphsCameraParameters &params, const Position &cameraPos)
{
    camera = coordinator->CreateEntity();

    coordinator->AddComponent<Position>(camera, cameraPos);
    coordinator->AddComponent<AnaglyphsCameraParameters>(camera, params);
}


alg::Mat4x4 AnaglyphsCameraSystem::ViewMatrix() const
{
    auto const& cameraPos = coordinator->GetComponent<Position>(camera);
    auto const& params = coordinator->GetComponent<AnaglyphsCameraParameters>(camera);

    alg::Vec3 direction = (cameraPos.vec - params.target.vec).Normalize();
    alg::Vec3 right = alg::Cross(globalUp, direction).Normalize();

    Position eyePos(cameraPos);

    switch (currentEye){
        case Eye::Left:
            eyePos.vec -= right * params.eyeSeparation * 0.5f;
            break;
        
        case Eye::Right:
            eyePos.vec += right * params.eyeSeparation * 0.5f;
            break;
    }

    return alg::LookAt(cameraPos.vec, direction, globalUp);
}


alg::Mat4x4 AnaglyphsCameraSystem::PerspectiveMatrix() const
{
    auto const& params = coordinator->GetComponent<AnaglyphsCameraParameters>(camera);

    float aspectRatio = params.GetAspectRatio();

    float top = std::tan(params.fov / 2.0f) * params.nearPlane;
    float bottom = -top;

    float convPlaneWidth = 2.f * params.convergence * aspectRatio * std::tan(params.fov / 2.0f);

    float left, right;

    switch (currentEye)
    {
    case Eye::Left:
        left = params.nearPlane * (convPlaneWidth + params.eyeSeparation) / (2.f * params.convergence);
        right = -params.nearPlane * (convPlaneWidth - params.eyeSeparation) / (2.f * params.convergence);
        break;

    case Eye::Right:
        left = params.nearPlane * (convPlaneWidth - params.eyeSeparation) / (2.f * params.convergence);
        right = -params.nearPlane * (convPlaneWidth + params.eyeSeparation) / (2.f * params.convergence);
        break;

    default:
        throw std::runtime_error("Unknown eye type");
    }

    return alg::Frustum(
        params.nearPlane,
        params.farPlane,
        left,
        right,
        top,
        bottom
    );
}
