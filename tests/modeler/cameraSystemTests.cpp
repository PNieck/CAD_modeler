#include <gtest/gtest.h>

#include <ecs/coordinator.hpp>
#include <CAD_modeler/model/components/registerComponents.hpp>
#include <CAD_modeler/model/systems/cameraSystem.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>


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
        .nearPlane = 0.1f,
        .farPlane = 100.0f,
    };

    cameraSystem->Init(params, Position(0.0f, 0.0f, 10.0f));

    EXPECT_EQ(0.1f, cameraSystem->GetParameters().nearPlane);
}


TEST(CameraSystemsTests, SimplePerspectiveMatrixCompareWithGLM) {
    Coordinator coordinator;

    RegisterAllComponents(coordinator);
    CameraSystem::RegisterSystem(coordinator);
    auto cameraSystem = coordinator.GetSystem<CameraSystem>();

    CameraParameters params {
        .target = Position(0.0f),
        .viewportWidth = 600,
        .viewportHeight = 400,
        .fov = glm::radians(45.0f),
        .nearPlane = 0.1f,
        .farPlane = 100.0f,
    };

    cameraSystem->Init(params, Position(0.0f, 0.0f, 10.0f));

    auto systemProjectionMatrix = cameraSystem->PerspectiveMatrix();
    auto glmMatrix = glm::perspective(
        params.fov,
        params.GetAspectRatio(),
        params.nearPlane,
        params.farPlane
    );

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            std::cout << systemProjectionMatrix(row, col) << " ";
        }
        std::cout << "\n";
    }

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            EXPECT_EQ(glmMatrix[row][col], systemProjectionMatrix(row, col));
        }
    }
}


TEST(CameraSystemsTests, SimpleViewMatrixCompareWithGLM) {
    Coordinator coordinator;

    RegisterAllComponents(coordinator);
    CameraSystem::RegisterSystem(coordinator);
    auto cameraSystem = coordinator.GetSystem<CameraSystem>();

    CameraParameters params {
        .target = Position(0.0f),
        .viewportWidth = 600,
        .viewportHeight = 400,
        .fov = glm::radians(45.0f),
        .nearPlane = 0.1f,
        .farPlane = 100.0f,
    };

    cameraSystem->Init(params, Position(0.0f, 0.0f, 10.0f));

    auto systemViewMatrix = cameraSystem->ViewMatrix();
    auto glmMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 10.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    std::cout << glm::to_string(glmMatrix) << "\n\n";

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            std::cout << systemViewMatrix(row, col) << " ";
        }
        std::cout << "\n";
    }

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            EXPECT_EQ(glmMatrix[row][col], systemViewMatrix(row, col));
        }
    }
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
        .nearPlane = 0.1f,
        .farPlane = 100.0f,
    };

    cameraSystem->Init(params, Position(-5.0f, 4.0f, 10.0f));

    auto systemViewMatrix = cameraSystem->ViewMatrix();
    auto glmMatrix = glm::lookAt(
        glm::vec3(-5.0f, 4.0f, 10.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    std::cout << glm::to_string(glmMatrix) << "\n\n";

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            std::cout << systemViewMatrix(row, col) << " ";
        }
        std::cout << "\n";
    }

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            EXPECT_NEAR(glmMatrix[row][col], systemViewMatrix(row, col), 0.001);
        }
    }
}
