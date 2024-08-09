#include <CAD_modeler/model/cameraManager.hpp>

#include <CAD_modeler/utilities/angle.hpp>
#include <CAD_modeler/model/systems/positionSystem.hpp>
#include <CAD_modeler/utilities/visitorHelper.hpp>

#include <stdexcept>


CameraManager::CameraManager(Coordinator &coordinator):
    coordinator(coordinator)
{
    CameraSystem::RegisterSystem(coordinator);
    AnaglyphsCameraSystem::RegisterSystem(coordinator);
    PositionSystem::RegisterSystem(coordinator);
}


void CameraManager::Init(int viewportWidth, int viewportHeight)
{
    auto perspectiveCamSys = coordinator.GetSystem<CameraSystem>();
    auto anaglyphsCamSys = coordinator.GetSystem<AnaglyphsCameraSystem>();

    Position initCamPos(0.0f, 0.0f, 10.0f);

    CameraParameters persCamParams {
        .target = Position(0.0f),
        .viewportWidth = viewportWidth,
        .viewportHeight = viewportHeight,
        .fov = Angle::FromDegrees(45.f).ToRadians(),
        .near_plane = 0.1f,
        .far_plane = 100.0f,
    };

    AnaglyphsCameraParameters anaCamParams(persCamParams, 0.5, 10.f);

    perspectiveCamSys->Init(persCamParams, initCamPos);
    anaglyphsCamSys->Init(anaCamParams, initCamPos);

    currentCameraSys = coordinator.GetSystem<CameraSystem>();
}


alg::Mat4x4 CameraManager::PerspectiveMtx() const
{
    alg::Mat4x4 result;

    std::visit(
        [&result] (const auto& sys) { result = sys->PerspectiveMatrix(); },
        currentCameraSys
    );

    return result;
}


alg::Mat4x4 CameraManager::ViewMtx() const
{
    alg::Mat4x4 result;

    std::visit(
        [&result] (const auto& sys) { result = sys->ViewMatrix(); },
        currentCameraSys
    );

    return result;
}


void CameraManager::RotateCamera(float x, float y)
{
    auto positionSys = coordinator.GetSystem<PositionSystem>();
    auto params = GetBaseParams();
    Entity camera = GetCameraEntity();

    positionSys->RotateAround(camera, params.target, x, y, 0.f);
}


float CameraManager::GetDistanceFromTarget() const
{
    auto positionSys = coordinator.GetSystem<PositionSystem>();
    auto params = GetBaseParams();
    Entity camera = GetCameraEntity();

    return positionSys->Distance(camera, params.target);
}


void CameraManager::SetDistanceFromTarget(float newDist)
{
    auto positionSys = coordinator.GetSystem<PositionSystem>();
    auto params = GetBaseParams();
    Entity camera = GetCameraEntity();

    positionSys->SetDistance(newDist, camera, params.target);
}


CameraManager::CameraType CameraManager::GetCurrentCameraType() const
{
    return std::visit(
        Overloaded {
            [] (PerspectiveSys) { return CameraType::Perspective; },
            [] (AnaglyphsSys) { return CameraType::Anaglyphs; }
        }, currentCameraSys
    );
}


void CameraManager::SetCameraType(CameraType camType)
{
    if (camType == GetCurrentCameraType())
        return;

    switch (camType)
    {
    case CameraType::Perspective:
        SwitchToPerspectiveCameraType();
        break;

    case CameraType::Anaglyphs:
        SwitchToAnaglyphsCameraType();
        break;
    
    default:
        throw std::runtime_error("Unknown camera type");
    }
}


void CameraManager::SwitchToPerspectiveCameraType()
{
    auto oldSys = std::get<AnaglyphsSys>(currentCameraSys);
    auto newSys = coordinator.GetSystem<CameraSystem>();

    AnaglyphsCameraParameters oldParams = oldSys->GetParameters();
    Position camPos = oldSys->GetCameraPos();

    CameraParameters newParams(oldParams);

    newSys->SetParameters(newParams);
    newSys->SetCameraPos(camPos);

    currentCameraSys = newSys;
}


void CameraManager::SwitchToAnaglyphsCameraType()
{
    auto oldSys = std::get<PerspectiveSys>(currentCameraSys);
    auto newSys = coordinator.GetSystem<AnaglyphsCameraSystem>();

    CameraParameters oldParams = oldSys->GetParameters();
    Position camPos = oldSys->GetCameraPos();

    AnaglyphsCameraParameters newParams(oldParams, eyeSeparation, convergence);

    newSys->SetParameters(newParams);
    newSys->SetCameraPos(camPos);

    currentCameraSys = newSys;
}


void CameraManager::SetCurrentEye(AnaglyphsCameraSystem::Eye eye)
{
    if (!std::holds_alternative<AnaglyphsSys>(currentCameraSys))
        return;

    auto anaglyphsSys = std::get<AnaglyphsSys>(currentCameraSys);
    anaglyphsSys->SetCurrentEye(eye);
}


void CameraManager::SetEyeSeparation(float eyeSeparation)
{
    this->eyeSeparation = eyeSeparation;

    if (GetCurrentCameraType() != CameraType::Anaglyphs)
        return;

    auto sys = std::get<AnaglyphsSys>(currentCameraSys);
    auto params = sys->GetParameters();
    params.eyeSeparation = eyeSeparation;
    sys->SetParameters(params);
}


void CameraManager::SetConvergence(float convergence)
{
    this->convergence = convergence;

    if (GetCurrentCameraType() != CameraType::Anaglyphs)
        return;

    auto sys = std::get<AnaglyphsSys>(currentCameraSys);
    auto params = sys->GetParameters();
    params.convergence = convergence;
    sys->SetParameters(params);
}


CameraParameters CameraManager::GetBaseParams() const
{
    return std::visit(
        [] (const auto& sys) -> CameraParameters { return sys->GetParameters(); },
        currentCameraSys
    );
}


void CameraManager::SetBaseParams(const CameraParameters &params)
{
    std::visit(
        Overloaded {
            [&params] (PerspectiveSys sys) { sys->SetParameters(params); },
            [&params] (AnaglyphsSys sys) { 
                auto oldParams = sys->GetParameters();
                AnaglyphsCameraParameters newParams(params, oldParams.eyeSeparation, oldParams.convergence);
                sys->SetParameters(newParams);
            }
        }, currentCameraSys
    );
}


Entity CameraManager::GetCameraEntity() const
{
    return std::visit(
        [] (const auto& sys) -> Entity { return sys->GetCameraEntity(); },
        currentCameraSys
    );
}