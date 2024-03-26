#include <CAD_modeler/model/systems/cameraSystem.hpp>

#include <CAD_modeler/model/components/cameraParameters.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/ecsCoordinator.hpp>

#include <glm/trigonometric.hpp>


void CameraSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<CameraSystem>();

    coordinator.RegisterComponent<CameraParameters>();
    coordinator.RegisterComponent<Position>();
}


void CameraSystem::Init(int viewport_width, int viewport_height)
{
    camera = coordinator->CreateEntity();

    Position pos(0.0f, 0.0f, 5.0f);
    coordinator->AddComponent<Position>(camera, pos);

    CameraParameters params {
        .target = glm::vec3(0.0f),
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

    glm::vec3 direction = glm::normalize(position.vec - params.target);
    glm::vec3 right = glm::normalize(glm::cross(globalUp, direction));
    glm::vec3 up = glm::normalize(glm::cross(direction, right));

    return glm::transpose(glm::mat4x4(
            right.x,     right.y,     right.z,     -position.X()*right.x -     position.Y()*right.y -     position.Z()*right.z,
               up.x,        up.y,        up.z,        -position.X()*up.x -        position.Y()*up.y -        position.Z()*up.z,
        direction.x, direction.y, direction.z, -position.X()*direction.x - position.Y()*direction.y - position.Z()*direction.z,
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
