#include <CAD_modeler/model.hpp>

#include <CAD_modeler/utilities/line.hpp>
#include <CAD_modeler/utilities/plane.hpp>

#include <stdexcept>


Model::Model(int viewport_width, int viewport_height)
{
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(std::numeric_limits<uint32_t>::max());

    CameraSystem::RegisterSystem(coordinator);
    MeshRenderer::RegisterSystem(coordinator);
    ToriSystem::RegisterSystem(coordinator);
    GridSystem::RegisterSystem(coordinator);
    CursorSystem::RegisterSystem(coordinator);
    PointsSystem::RegisterSystem(coordinator);
    NameSystem::RegisterSystem(coordinator);
    SelectionSystem::RegisterSystem(coordinator);

    cameraSys = coordinator.GetSystem<CameraSystem>();
    meshRenderer = coordinator.GetSystem<MeshRenderer>();
    toriSystem = coordinator.GetSystem<ToriSystem>();
    gridSystem = coordinator.GetSystem<GridSystem>();
    cursorSystem = coordinator.GetSystem<CursorSystem>();
    pointsSystem = coordinator.GetSystem<PointsSystem>();
    nameSystem = coordinator.GetSystem<NameSystem>();
    selectionSystem = coordinator.GetSystem<SelectionSystem>();

    cameraSys->Init(viewport_width, viewport_height);
    gridSystem->Init();
    cursorSystem->Init();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}


void Model::RenderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);

    gridSystem->Render();
    meshRenderer->Render();
    cursorSystem->Render();
    pointsSystem->Render();
}


void Model::AddTorus()
{
    Position cursorPos = cursorSystem->GetPosition();

    // Default torus parameters
    TorusParameters params {
        .majorRadius = 1.0f,
        .minorRadius = 0.2f,
        .pointsInCirc = 6,
        .circCnt = 5,
    };

    toriSystem->AddTorus(cursorPos, params);
}


void Model::ChangeViewportSize(int width, int height)
{
    glViewport(0, 0, width, height);
    cameraSys->ChangeViewportSize(width, height);
}


void Model::SetCursorPositionFromViewport(float x, float y) const
{
    cursorSystem->SetPosition(PointFromViewportCoordinates(x, y));
}


void Model::Add3DPointFromViewport(float x, float y) const
{
    Position newPos(PointFromViewportCoordinates(x, y));
    pointsSystem->CreatePoint(newPos);
}


glm::vec3 Model::PointFromViewportCoordinates(float x, float y) const
{
    auto viewMtx = cameraSys->ViewMatrix();
    auto perspectiveMtx = cameraSys->PerspectiveMatrix();
    auto const& cameraTarget = cameraSys->GetTargetPosition();
    auto const& cameraPos = cameraSys->GetPosition();
    auto cursorPos = cursorSystem->GetPosition();

    auto cameraInv = glm::inverse(perspectiveMtx * viewMtx);

    glm::vec4 nearV4 = cameraInv * glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 farV4 = cameraInv * glm::vec4(x, y, 1.0f, 1.0f);

    glm::vec3 near = glm::vec3(
        nearV4.x / nearV4.w,
        nearV4.y / nearV4.w,
        nearV4.z / nearV4.w
    );

    glm::vec3 far = glm::vec3(
        farV4.x / farV4.w,
        farV4.y / farV4.w,
        farV4.z / farV4.w
    );

    Line nearToFar = Line::FromTwoPoints(near, far);

    glm::vec3 cameraDirection = cameraTarget.vec - cameraPos.vec;
    Plane perpendicularToScreenWithCursor(cursorPos.vec, cameraDirection);

    std::optional<glm::vec3> intersection =
        perpendicularToScreenWithCursor.Intersect(nearToFar);

    if (!intersection.has_value()) {
        throw std::runtime_error("Cannot project viewport coordinates to 3d ones");
    }

    return intersection.value();
}
