#include <CAD_modeler/model.hpp>

#include <CAD_modeler/utilities/line.hpp>
#include <CAD_modeler/utilities/plane.hpp>
#include <CAD_modeler/utilities/angle.hpp>

#include <CAD_modeler/model/components/registerComponents.hpp>
#include <CAD_modeler/model/components/cameraParameters.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>
#include <CAD_modeler/model/systems/curveControlPointsSystem.hpp>

#include <stdexcept>


Model::Model(int viewport_width, int viewport_height):
    cameraManager(coordinator)
{
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(std::numeric_limits<uint32_t>::max());

    RegisterAllComponents(coordinator);

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
    ControlPointsRegistrySystem::RegisterSystem(coordinator);
    GregoryPatchesSystem::RegisterSystem(coordinator);

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
    controlPointsRegistrySys = coordinator.GetSystem<ControlPointsRegistrySystem>();
    gregoryPatchesSystem = coordinator.GetSystem<GregoryPatchesSystem>();

    cameraManager.Init(viewport_width, viewport_height);
    gridSystem->Init(&shadersRepo);
    cursorSystem->Init(&shadersRepo);
    selectionSystem->Init(&shadersRepo);
    pointsSystem->Init(&shadersRepo);
    toriSystem->Init(&shadersRepo);
    c0CurveSystem->Init(&shadersRepo);
    c2CurveSystem->Init(&shadersRepo);
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
    switch (cameraManager.GetCurrentCameraType())
    {
    case CameraManager::CameraType::Perspective:
        RenderPerspectiveFrame();
        break;

    case CameraManager::CameraType::Anaglyphs:
        RenderAnaglyphsFrame();
        break;
    
    default:
        throw std::runtime_error("Unknown camera type");
    }
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


void Model::MergeControlPoints(Entity e1, Entity e2)
{
    auto registrySys = coordinator.GetSystem<ControlPointsRegistrySystem>();
    auto curveCPSys = coordinator.GetSystem<CurveControlPointsSystem>();

    auto ownersSet = registrySys->GetOwnersOfControlPoints(e2);

    for (auto owner: ownersSet) {
        auto sysId = std::get<SystemId>(owner);

        if (sysId == Coordinator::GetSystemID<C0CurveSystem>() ||
            sysId == Coordinator::GetSystemID<C2CurveSystem>() ||
            sysId == Coordinator::GetSystemID<InterpolationCurveSystem>())
            curveCPSys->MergeControlPoints(std::get<Entity>(owner), e2, e1, sysId);

        else if (sysId == Coordinator::GetSystemID<C0CylinderSystem>() ||
                 sysId == Coordinator::GetSystemID<C0SurfaceSystem>())
                 c0PatchesSystem->MergeControlPoints(std::get<Entity>(owner), e2, e1, sysId);

        else if (sysId == Coordinator::GetSystemID<C2SurfaceSystem>())
                 c2SurfaceSystem->MergeControlPoints(std::get<Entity>(owner), e2, e1);

        else if (sysId == Coordinator::GetSystemID<C2CylinderSystem>())
                 c2CylinderSystem->MergeControlPoints(std::get<Entity>(owner), e2, e1);
        else
            throw std::runtime_error("Unknown system");
    }

    auto const& pos1 = coordinator.GetComponent<Position>(e1);
    auto const& pos2 = coordinator.GetComponent<Position>(e2);
    Position newPos((pos1.vec + pos2.vec) * 0.5f);

    coordinator.SetComponent<Position>(e1, newPos);
    coordinator.DestroyEntity(e2);
}


void Model::ChangeViewportSize(int width, int height)
{
    glViewport(0, 0, width, height);

    auto params = cameraManager.GetBaseParams();
    params.viewportWidth = width;
    params.viewportHeight = height;

    cameraManager.SetBaseParams(params);
}


Entity Model::Add3DPointFromViewport(float x, float y)
{
    Position newPos(PointFromViewportCoordinates(x, y));
    return pointsSystem->CreatePoint(newPos);
}


void Model::TryToSelectFromViewport(float x, float y)
{
    Line line(LineFromViewportCoordinates(x, y));
    selectionSystem->SelectFromLine(line);
}


std::vector<GregoryPatchesSystem::Hole> Model::GetHolesPossibleToFill(const std::unordered_set<Entity> &entities) const
{
    std::vector<C0Patches> c0Patches;
    c0Patches.reserve(entities.size());

    for (auto entity: entities)
        c0Patches.push_back(coordinator.GetComponent<C0Patches>(entity));

    return gregoryPatchesSystem->FindHolesToFill(c0Patches);
}


alg::Vec3 Model::PointFromViewportCoordinates(float x, float y)
{
    auto cameraParams = cameraManager.GetBaseParams();
    auto const& cameraTarget = cameraParams.target;
    auto const& cameraPos = cameraManager.GetCameraPosition();
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


Line Model::LineFromViewportCoordinates(float x, float y)
{
    auto cameraType = cameraManager.GetCurrentCameraType();
    if (cameraType == CameraManager::CameraType::Anaglyphs)
        cameraManager.SetCameraType(CameraManager::CameraType::Perspective);

    auto viewMtx = cameraManager.ViewMtx();
    auto perspectiveMtx = cameraManager.PerspectiveMtx();

    cameraManager.SetCameraType(cameraType);

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


void Model::RenderAnaglyphsFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Render picture for the left eye
    cameraManager.SetCurrentEye(CameraManager::Eye::Left);
    alg::Mat4x4 persMtx = cameraManager.PerspectiveMtx();
    alg::Mat4x4 viewMtx = cameraManager.ViewMtx();
    auto camParams = cameraManager.GetBaseParams();

    glColorMask(true, false, false, false);
    RenderSystemsObjects(viewMtx, persMtx, camParams.nearPlane, camParams.farPlane);

    // Render picture for right eye
    cameraManager.SetCurrentEye(CameraManager::Eye::Right);
    persMtx = cameraManager.PerspectiveMtx();
    viewMtx = cameraManager.ViewMtx();

    glColorMask(false, true, true, false);
    RenderSystemsObjects(viewMtx, persMtx, camParams.nearPlane, camParams.farPlane);

    // Setting mask back to normal
    glColorMask(true, true, true, true);
}


void Model::RenderPerspectiveFrame()
{
    alg::Mat4x4 persMtx = cameraManager.PerspectiveMtx();
    alg::Mat4x4 viewMtx = cameraManager.ViewMtx();
    auto camParams = cameraManager.GetBaseParams();

    glClear(GL_COLOR_BUFFER_BIT);

    RenderSystemsObjects(viewMtx, persMtx, camParams.nearPlane, camParams.farPlane);
}


void Model::RenderSystemsObjects(const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, float nearPlane, float farPlane) const
{
    alg::Mat4x4 cameraMtx = persMtx * viewMtx;

    gridSystem->Render(viewMtx, persMtx, nearPlane, farPlane);
    toriSystem->Render(cameraMtx);
    cursorSystem->Render(cameraMtx);
    pointsSystem->Render(cameraMtx);
    selectionSystem->RenderMiddlePoint(cameraMtx);
    c0CurveSystem->Render(cameraMtx);
    c2CurveSystem->Render(cameraMtx);
    interpolationCurveSystem->Render(cameraMtx);
    c0PatchesSystem->Render(cameraMtx);
    c2SurfaceSystem->Render(cameraMtx);
    c2CylinderSystem->Render(cameraMtx);
    controlNetSystem->Render(cameraMtx);
}
