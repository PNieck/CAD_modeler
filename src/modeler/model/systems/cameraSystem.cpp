#include <CAD_modeler/model/systems/cameraSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/cameraParameters.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/rotation.hpp>

#include <glm/trigonometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>


void CameraSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<CameraSystem>();

    coordinator.RegisterComponent<CameraParameters>();
    coordinator.RegisterComponent<Position>();
}


void CameraSystem::Init(int viewport_width, int viewport_height)
{
    camera = coordinator->CreateEntity();

    Position pos(0.0f, 0.0f, 10.0f);
    coordinator->AddComponent<Position>(camera, pos);

    CameraParameters params {
        .target = Position(0.0f),
        .aspect_ratio = (float)viewport_width/(float)viewport_height,
        .fov = glm::radians(45.0f),
        .near_plane = 0.1f,
        .far_plane = 100.0f,
    };
    coordinator->AddComponent<CameraParameters>(camera, params);
}


glm::mat4x4 CameraSystem::ViewMatrix() const
{
    Position& position = coordinator->GetComponent<Position>(camera);
    CameraParameters& params = coordinator->GetComponent<CameraParameters>(camera);

    glm::vec3 direction = glm::normalize(position.vec - params.target.vec);
    glm::vec3 right = glm::normalize(glm::cross(globalUp, direction));
    glm::vec3 up = glm::normalize(glm::cross(direction, right));

    return glm::transpose(glm::mat4x4(
            right.x,     right.y,     right.z,     -position.GetX()*right.x -     position.GetY()*right.y -     position.GetZ()*right.z,
               up.x,        up.y,        up.z,        -position.GetX()*up.x -        position.GetY()*up.y -        position.GetZ()*up.z,
        direction.x, direction.y, direction.z, -position.GetX()*direction.x - position.GetY()*direction.y - position.GetZ()*direction.z,
               0.0f,        0.0f,        0.0f,                                                                      1.0f
    ));
}


glm::mat4x4 CameraSystem::PerspectiveMatrix() const
{
    CameraParameters& params = coordinator->GetComponent<CameraParameters>(camera);

    float v1 = 1.0f/std::tan(params.fov/2.0);
    float v2 = v1/params.aspect_ratio;
    float v3 = (params.far_plane + params.near_plane)/(params.far_plane - params.near_plane);
    float v4 = -2.0 * (params.far_plane * params.near_plane) / (params.far_plane - params.near_plane);

    return glm::transpose(glm::mat4x4(
        v2, 0.0f, 0.0f, 0.0f,
        0.0f, v1, 0.0f, 0.0f,
        0.0f, 0.0f, -v3, v4,
        0.0f, 0.0f, -1.0f, 0.0f
    ));
}


void CameraSystem::RotateAroundTarget(float x, float y) const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    Position& position = coordinator->GetComponent<Position>(camera);

    glm::mat4x4 translationMtx = params.target.TranslationMatrix();
    glm::mat4x4 translationInv = glm::inverse(translationMtx);

    Rotation rot(x, y, 0.0f);

    glm::vec4 pos(position.vec, 1.0f);

    pos = translationMtx * rot.GetRotationMatrix() * translationInv * pos;

    position.SetX(pos.x);
    position.SetY(pos.y);
    position.SetZ(pos.z);
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

    return static_cast<float>((params.target.vec - position.vec).length());
}


void CameraSystem::SetDistanceToTarget(float newDist) const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    Position& position = coordinator->GetComponent<Position>(camera);

    position.vec = glm::normalize(params.target.vec - position.vec) * newDist + params.target.vec;
}


void CameraSystem::MultiplyDistanceToTarget(float coefficient) const
{
    CameraParameters const& params = coordinator->GetComponent<CameraParameters>(camera);
    Position& position = coordinator->GetComponent<Position>(camera);

    position.vec = (position.vec - params.target.vec) * coefficient;
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


void CameraSystem::ChangeViewportSize(int width, int height) const
{
    CameraParameters& params = coordinator->GetComponent<CameraParameters>(camera);
    params.aspect_ratio = (float)width/(float)height;
}
