#include <CAD_modeler/model.hpp>

#include <CAD_modeler/utilities/line.hpp>
#include <CAD_modeler/utilities/plane.hpp>
#include <CAD_modeler/utilities/angle.hpp>

#include <CAD_modeler/model/components/registerComponents.hpp>
#include <CAD_modeler/model/components/cameraParameters.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>
#include <CAD_modeler/model/systems/curveControlPointsSystem.hpp>

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
    CurveControlPointsSystem::RegisterSystem(coordinator);
    C0CurveSystem::RegisterSystem(coordinator);
    C2CurveSystem::RegisterSystem(coordinator);
    ToUpdateSystem::RegisterSystem(coordinator);
    InterpolationCurveSystem::RegisterSystem(coordinator);
    C0SurfaceSystem::RegisterSystem(coordinator);
    C0CylinderSystem::RegisterSystem(coordinator);
    C0PatchesSystem::RegisterSystem(coordinator);
    C2SurfaceSystem::RegisterSystem(coordinator);
    C2CylinderSystem::RegisterSystem(coordinator);
    ControlNetSystem::RegisterSystem(coordinator);

    cameraSys = coordinator.GetSystem<CameraSystem>();
    toriSystem = coordinator.GetSystem<ToriSystem>();
    gridSystem = coordinator.GetSystem<GridSystem>();
    cursorSystem = coordinator.GetSystem<CursorSystem>();
    pointsSystem = coordinator.GetSystem<PointsSystem>();
    nameSystem = coordinator.GetSystem<NameSystem>();
    selectionSystem = coordinator.GetSystem<SelectionSystem>();
    c0CurveSystem = coordinator.GetSystem<C0CurveSystem>();
    c2CurveSystem = coordinator.GetSystem<C2CurveSystem>();
    auto controlPointsSystem = coordinator.GetSystem<CurveControlPointsSystem>();
    interpolationCurveSystem = coordinator.GetSystem<InterpolationCurveSystem>();
    c0SurfaceSystem = coordinator.GetSystem<C0SurfaceSystem>();
    c0CylinderSystem = coordinator.GetSystem<C0CylinderSystem>();
    c0PatchesSystem = coordinator.GetSystem<C0PatchesSystem>();
    c2SurfaceSystem = coordinator.GetSystem<C2SurfaceSystem>();
    c2CylinderSystem = coordinator.GetSystem<C2CylinderSystem>();
    controlNetSystem = coordinator.GetSystem<ControlNetSystem>();

    CameraParameters params {
        .target = Position(0.0f),
        .viewportWidth = viewport_width,
        .viewportHeight = viewport_height,
        .fov = Angle::FromDegrees(45.f).ToRadians(),
        .near_plane = 0.1f,
        .far_plane = 100.0f,
    };

    cameraSys->Init(params, Position(0.0f, 0.0f, 10.0f));
    gridSystem->Init(&shadersRepo);
    cursorSystem->Init(&shadersRepo);
    selectionSystem->Init(&shadersRepo);
    pointsSystem->Init(&shadersRepo);
    toriSystem->Init(&shadersRepo);
    c0CurveSystem->Init(&shadersRepo);
    c2CurveSystem->Init(&shadersRepo);
    controlPointsSystem->Init();
    interpolationCurveSystem->Init(&shadersRepo);
    c0PatchesSystem->Init(&shadersRepo);
    c0SurfaceSystem->Init();
    c0CylinderSystem->Init();
    c2SurfaceSystem->Init(&shadersRepo);
    c2CylinderSystem->Init(&shadersRepo);
    controlNetSystem->Init(&shadersRepo);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}


void Model::RenderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);

    gridSystem->Render();
    toriSystem->Render();
    cursorSystem->Render();
    pointsSystem->Render();
    selectionSystem->RenderMiddlePoint();
    c0CurveSystem->Render();
    c2CurveSystem->Render();
    interpolationCurveSystem->Render();
    c0PatchesSystem->Render();
    c2SurfaceSystem->Render();
    c2CylinderSystem->Render();
    controlNetSystem->Render();
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


Entity Model::Add3DPointFromViewport(float x, float y) const
{
    Position newPos(PointFromViewportCoordinates(x, y));
    return pointsSystem->CreatePoint(newPos);
}


void Model::TryToSelectFromViewport(float x, float y) const
{
    Line line(LineFromViewportCoordinates(x, y));
    selectionSystem->SelectFromLine(line);
}


alg::Vec3 Model::PointFromViewportCoordinates(float x, float y) const
{
    auto const& cameraTarget = cameraSys->GetTargetPosition();
    auto const& cameraPos = cameraSys->GetPosition();
    auto cursorPos = cursorSystem->GetPosition();

    Line nearToFar = LineFromViewportCoordinates(x, y);

    alg::Vec3 cameraDirection = cameraTarget.vec - cameraPos.vec;
    Plane perpendicularToScreenWithCursor(cursorPos.vec, cameraDirection);

    std::optional<alg::Vec3> intersection =
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

    auto cameraInv = (perspectiveMtx * viewMtx).Inverse().value();

    alg::Vec4 nearV4 = cameraInv * alg::Vec4(x, y, 1.0f, 1.0f);
    alg::Vec4 farV4 = cameraInv * alg::Vec4(x, y, -1.0f, 1.0f);

    alg::Vec3 near = alg::Vec3(
        nearV4.X() / nearV4.W(),
        nearV4.Y() / nearV4.W(),
        nearV4.Z() / nearV4.W()
    );

    alg::Vec3 far = alg::Vec3(
        farV4.X() / farV4.W(),
        farV4.Y() / farV4.W(),
        farV4.Z() / farV4.W()
    );

    return Line::FromTwoPoints(near, far);
}
