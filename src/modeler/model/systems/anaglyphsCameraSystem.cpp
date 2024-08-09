#include <CAD_modeler/model/systems/anaglyphsCameraSystem.hpp>


void AnaglyphsCameraSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<AnaglyphsCameraSystem>();
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

    float aspectRatio = static_cast<float>(params.viewportWidth) / static_cast<float>(params.viewportHeight);

    float top = std::tan(params.fov / 2.0f) * params.near_plane;
    float bottom = -top;

    float a = aspectRatio * std::tan(params.fov / 2.0f) * params.convergence;
    float b = a - params.eyeSeparation/2.0f;
    float c = a + params.eyeSeparation/2.0f;

    float left, right;

    switch (currentEye)
    {
    case Eye::Left:
        left = -b * params.near_plane/params.convergence;
        right = c * params.near_plane/params.convergence;
        break;

    case Eye::Right:
        left = -c * params.near_plane/params.convergence;
        right = b * params.near_plane/params.convergence;
        break;
    }


    return alg::Frustum(
        params.near_plane,
        params.far_plane,
        left,
        right,
        top,
        bottom
    );
}
