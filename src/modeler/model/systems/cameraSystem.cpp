#include <CAD_modeler/model/systems/cameraSystem.hpp>

#include <ecs/coordinator.hpp>

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
    alg::Vec3 right = alg::Cross(globalUp, direction).Normalize();
    alg::Vec3 up = alg::Cross(direction, right).Normalize();

    alg::Mat4x4 result(
            right.X(),     right.Y(),     right.Z(),     -position.GetX()*right.X() -     position.GetY()*right.Y() -     position.GetZ()*right.Z(),
               up.X(),        up.Y(),        up.Z(),        -position.GetX()*up.X() -        position.GetY()*up.Y() -        position.GetZ()*up.Z(),
        direction.X(), direction.Y(), direction.Z(), -position.GetX()*direction.X() - position.GetY()*direction.Y() - position.GetZ()*direction.Z(),
                 0.0f,          0.0f,          0.0f,                                                                                           1.0f
    );

    result.TransposeSelf();

    return result;
}


alg::Mat4x4 CameraSystem::PerspectiveMatrix() const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);

    float aspectRatio = static_cast<float>(params.viewportWidth) / static_cast<float>(params.viewportHeight);

    float v1 = 1.0f/std::tan(params.fov/2.0);
    float v2 = v1/aspectRatio;
    float v3 = (params.far_plane + params.near_plane)/(params.far_plane - params.near_plane);
    float v4 = -2.0 * (params.far_plane * params.near_plane) / (params.far_plane - params.near_plane);

    alg::Mat4x4 result(
        v2, 0.0f, 0.0f, 0.0f,
        0.0f, v1, 0.0f, 0.0f,
        0.0f, 0.0f, -v3, v4,
        0.0f, 0.0f, -1.0f, 0.0f
    );

    result.TransposeSelf();

    return result;
}


void CameraSystem::RotateAroundTarget(float x, float y) const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    Position const& position = coordinator->GetComponent<Position>(camera);

    alg::Mat4x4 translationMtx = params.target.TranslationMatrix();
    alg::Mat4x4 translationInv = translationMtx.Inverse().value();

    Rotation rot(x, y, 0.0f);

    alg::Vec4 pos(position.vec, 1.0f);

    pos = translationMtx * rot.GetRotationMatrix() * translationInv * pos;

    coordinator->SetComponent<Position>(camera, Position(pos.X(), pos.Y(), pos.Z()));
}


Position CameraSystem::GetPosition() const
{
    return coordinator->GetComponent<Position>(camera);
}


Position CameraSystem::GetTargetPosition() const
{
    return coordinator->GetComponent<CameraParameters>(camera).target;
}


float CameraSystem::GetDistanceToTarget() const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    Position const& position = coordinator->GetComponent<Position>(camera);

    return (params.target.vec - position.vec).Length();
}


void CameraSystem::SetDistanceToTarget(float newDist) const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    Position const& position = coordinator->GetComponent<Position>(camera);

    Position newPos((params.target.vec - position.vec).Normalize() * newDist + params.target.vec);
    coordinator->SetComponent<Position>(camera, newPos);
}


void CameraSystem::MultiplyDistanceToTarget(float coefficient) const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    Position const& position = coordinator->GetComponent<Position>(camera);

    Position newPos((position.vec - params.target.vec) * coefficient);
    coordinator->SetComponent<Position>(camera, newPos);
}


float CameraSystem::GetNearPlane() const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    return params.near_plane;
}


float CameraSystem::GetFarPlane() const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    return params.far_plane;
}

int CameraSystem::GetViewportWidth() const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    return params.viewportWidth;
}


int CameraSystem::GetViewportHeight() const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    return params.viewportHeight;
}


float CameraSystem::GetFov() const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    return params.fov;
}


void CameraSystem::ChangeViewportSize(int width, int height) const
{
    CameraParameters params = coordinator->GetComponent<CameraParameters>(camera);
    params.viewportHeight = height;
    params.viewportWidth = width;

    coordinator->SetComponent<CameraParameters>(camera, params);
}
