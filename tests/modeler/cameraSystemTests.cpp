#include <gtest/gtest.h>

#include <ecs/coordinator.hpp>
#include <CAD_modeler/model/components/registerComponents.hpp>
#include <CAD_modeler/model/systems/cameraSystem.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


TEST(CameraSystemsTests, Initialization) {
    Coordinator coordinator;

    RegisterAllComponents(coordinator);
    CameraSystem::RegisterSystem(coordinator);
    auto cameraSystem = coordinator.GetSystem<CameraSystem>();

    CameraParameters params {
        .target = Position(0.0f),
        .viewportWidth = 600,
        .viewportHeight = 400,
        .fov = glm::radians(45.0f),
        .near_plane = 0.1f,
        .far_plane = 100.0f,
    };

    cameraSystem->Init(params, Position(0.0f, 0.0f, 10.0f));

    EXPECT_EQ(0.1f, cameraSystem->GetNearPlane());
}


TEST(CameraSystemsTests, PerspectiveMatrixCompareWithGLM) {
    Coordinator coordinator;

    RegisterAllComponents(coordinator);
    CameraSystem::RegisterSystem(coordinator);
    auto cameraSystem = coordinator.GetSystem<CameraSystem>();

    CameraParameters params {
        .target = Position(0.0f),
        .viewportWidth = 600,
        .viewportHeight = 400,
        .fov = glm::radians(45.0f),
        .near_plane = 0.1f,
        .far_plane = 100.0f,
    };

    cameraSystem->Init(params, Position(0.0f, 0.0f, 10.0f));

    auto systemProjectionMatrix = cameraSystem->PerspectiveMatrix();
    auto glmMatrix = glm::perspective(
        cameraSystem->GetFov(),
        (float)cameraSystem->GetViewportWidth()/(float)cameraSystem->GetViewportHeight(),
        cameraSystem->GetNearPlane(),
        cameraSystem->GetFarPlane()
    );

    EXPECT_EQ(glmMatrix, systemProjectionMatrix);
}


TEST(CameraSystemsTests, ViewMatrixCompareWithGLM) {
    Coordinator coordinator;

    RegisterAllComponents(coordinator);
    CameraSystem::RegisterSystem(coordinator);
    auto cameraSystem = coordinator.GetSystem<CameraSystem>();

    CameraParameters params {
        .target = Position(0.0f),
        .viewportWidth = 600,
        .viewportHeight = 400,
        .fov = glm::radians(45.0f),
        .near_plane = 0.1f,
        .far_plane = 100.0f,
    };

    cameraSystem->Init(params, Position(0.0f, 0.0f, 10.0f));

    auto systemViewMatrix = cameraSystem->ViewMatrix();
    auto glmMatrix = glm::lookAt(
        cameraSystem->GetPosition().vec,
        cameraSystem->GetTargetPosition().vec,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    EXPECT_EQ(glmMatrix, systemViewMatrix);
}
