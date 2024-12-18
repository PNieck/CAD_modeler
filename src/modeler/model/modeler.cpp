#include <CAD_modeler/model/modeler.hpp>

#include <CAD_modeler/utilities/line.hpp>
#include <CAD_modeler/utilities/plane.hpp>

#include <CAD_modeler/model/components/registerComponents.hpp>
#include <CAD_modeler/model/components/cameraParameters.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>
#include <CAD_modeler/model/systems/curveControlPointsSystem.hpp>

#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>

#include <stdexcept>


Modeler::Modeler(const int viewportWidth, const int viewportHeight):
    Model(viewportWidth, viewportHeight)
{
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
    VectorSystem::RegisterSystem(coordinator);
    IntersectionSystem::RegisterSystem(coordinator);

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
    vectorSystem = coordinator.GetSystem<VectorSystem>();
    intersectionSystem = coordinator.GetSystem<IntersectionSystem>();

    ShaderRepository& shadersRepo = ShaderRepository::GetInstance();

    cameraManager.Init(viewportWidth, viewportHeight);
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
    gregoryPatchesSystem->Init(&shadersRepo);
    vectorSystem->Init(&shadersRepo);
}


void Modeler::AddTorus()
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


void Modeler::MergeControlPoints(Entity e1, Entity e2)
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


Entity Modeler::Add3DPointFromViewport(float x, float y)
{
    Position newPos(PointFromViewportCoordinates(x, y));
    return pointsSystem->CreatePoint(newPos);
}


void Modeler::TryToSelectFromViewport(float x, float y)
{
    Line line(LineFromViewportCoordinates(x, y));
    selectionSystem->SelectFromLine(line);
}


void Modeler::SelectMultipleFromViewport(float x, float y, float dist)
{
    Line line(LineFromViewportCoordinates(x, y));
    selectionSystem->SelectAllFromLine(line, dist);
}


std::vector<GregoryPatchesSystem::Hole> Modeler::GetHolesPossibleToFill(const std::unordered_set<Entity> &entities) const
{
    std::vector<C0Patches> c0Patches;
    c0Patches.reserve(entities.size());

    for (auto entity: entities)
        c0Patches.push_back(coordinator.GetComponent<C0Patches>(entity));

    return gregoryPatchesSystem->FindHolesToFill(c0Patches);
}


alg::Vec3 Modeler::PointFromViewportCoordinates(float x, float y)
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


Line Modeler::LineFromViewportCoordinates(float x, float y)
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


void Modeler::RenderSystemsObjects(const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, float nearPlane, float farPlane) const
{
    alg::Mat4x4 cameraMtx = persMtx * viewMtx;

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
    gregoryPatchesSystem->Render(cameraMtx);
    vectorSystem->Render(cameraMtx);
    gridSystem->Render(viewMtx, persMtx, nearPlane, farPlane);

    if (selectingEntities)
        selectionSystem->RenderSelectionCircle(
            selectionCircleX,
            selectionCircleY
        );
}
