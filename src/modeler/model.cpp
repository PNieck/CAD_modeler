#include <CAD_modeler/model.hpp>

#include <CAD_modeler/utilities/line.hpp>
#include <CAD_modeler/utilities/plane.hpp>

#include <CAD_modeler/model/components/registerComponents.hpp>
#include <CAD_modeler/model/components/cameraParameters.hpp>

#include <stdexcept>


Model::Model(int viewport_width, int viewport_height)
{
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(std::numeric_limits<uint32_t>::max());

    RegisterAllComponents(coordinator);

    CameraSystem::RegisterSystem(coordinator);
    ToriSystem::RegisterSystem(coordinator);
    GridSystem::RegisterSystem(coordinator);
    CursorSystem::RegisterSystem(coordinator);
    PointsSystem::RegisterSystem(coordinator);
    NameSystem::RegisterSystem(coordinator);
    SelectionSystem::RegisterSystem(coordinator);
    BezierCurveSystem::RegisterSystem(coordinator);

    cameraSys = coordinator.GetSystem<CameraSystem>();
    toriSystem = coordinator.GetSystem<ToriSystem>();
    gridSystem = coordinator.GetSystem<GridSystem>();
    cursorSystem = coordinator.GetSystem<CursorSystem>();
    pointsSystem = coordinator.GetSystem<PointsSystem>();
    nameSystem = coordinator.GetSystem<NameSystem>();
    selectionSystem = coordinator.GetSystem<SelectionSystem>();
    bezierCurveSystem = coordinator.GetSystem<BezierCurveSystem>();

    CameraParameters params {
        .target = Position(0.0f),
        .viewportWidth = viewport_width,
        .viewportHeight = viewport_height,
        .fov = glm::radians(45.0f),
        .near_plane = 0.1f,
        .far_plane = 100.0f,
    };

    cameraSys->Init(params, Position(0.0f, 0.0f, 10.0f));
    gridSystem->Init(&shadersRepo);
    cursorSystem->Init(&shadersRepo);
    selectionSystem->Init(&shadersRepo);
    pointsSystem->Init(&shadersRepo);
    toriSystem->Init(&shadersRepo);
    bezierCurveSystem->Init(&shadersRepo);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    Entity point1 = pointsSystem->CreatePoint(Position(-1.f, -1.f, -1.f));
    Entity point2 = pointsSystem->CreatePoint(Position(2.f, 2.f, 5.f));
    Entity point3 = pointsSystem->CreatePoint(Position(3.f, 3.f, 10.f));
    Entity point4 = pointsSystem->CreatePoint(Position(4.f, 4.f, 4.f));

    bezierCurveSystem->CreateBezierCurve({point1, point2, point3, point4});
}


void Model::RenderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);

    gridSystem->Render();
    toriSystem->Render();
    cursorSystem->Render();
    pointsSystem->Render();
    selectionSystem->RenderMiddlePoint();
    bezierCurveSystem->Render();
}


void Model::AddTorus()
{
    Position cursorPos = cursorSystem->GetPosition();

    // Default torus parameters
    TorusParameters params {
        .majorRadius = 1.0f,
        .minorRadius = 0.2f,
        .meshDensityMinR = 6,
        .meshDensityMajR = 5,
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


void Model::TryToSelectFromViewport(float x, float y) const
{
    Line line(LineFromViewportCoordinates(x, y));
    selectionSystem->SelectFromLine(line);
}


glm::vec3 Model::PointFromViewportCoordinates(float x, float y) const
{
    auto const& cameraTarget = cameraSys->GetTargetPosition();
    auto const& cameraPos = cameraSys->GetPosition();
    auto cursorPos = cursorSystem->GetPosition();

    Line nearToFar = LineFromViewportCoordinates(x, y);

    glm::vec3 cameraDirection = cameraTarget.vec - cameraPos.vec;
    Plane perpendicularToScreenWithCursor(cursorPos.vec, cameraDirection);

    std::optional<glm::vec3> intersection =
        perpendicularToScreenWithCursor.Intersect(nearToFar);

    if (!intersection.has_value()) {
        throw std::runtime_error("Cannot project viewport coordinates to 3d ones");
    }

    return intersection.value();
}


Line Model::LineFromViewportCoordinates(float x, float y) const
{
    auto viewMtx = cameraSys->ViewMatrix();
    auto perspectiveMtx = cameraSys->PerspectiveMatrix();

    auto cameraInv = glm::inverse(perspectiveMtx * viewMtx);

    glm::vec4 nearV4 = cameraInv * glm::vec4(x, y, 1.0f, 1.0f);
    glm::vec4 farV4 = cameraInv * glm::vec4(x, y, -1.0f, 1.0f);

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

    return Line::FromTwoPoints(near, far);
}
