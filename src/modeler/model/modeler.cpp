#include <CAD_modeler/model/modeler.hpp>

#include <CAD_modeler/utilities/line.hpp>
#include <CAD_modeler/utilities/plane.hpp>

#include <CAD_modeler/model/components/registerComponents.hpp>
#include <CAD_modeler/model/components/cameraParameters.hpp>
#include <CAD_modeler/model/components/unremovable.hpp>
#include <CAD_modeler/model/components/drawTrimmed.hpp>
#include <CAD_modeler/model/components/drawStd.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>
#include <CAD_modeler/model/systems/curveControlPointsSystem.hpp>
#include "CAD_modeler/model/systems/uvVisualizer.hpp"

#include <stdexcept>


Modeler::Modeler(const int viewportWidth, const int viewportHeight):
    Model(viewportWidth, viewportHeight)
{
    RegisterAllComponents(coordinator);

    ToriSystem::RegisterSystem(coordinator);
    ToriRenderingSystem::RegisterSystem(coordinator);
    TrimmedToriRenderingSystem::RegisterSystem(coordinator);
    GridSystem::RegisterSystem(coordinator);
    CursorSystem::RegisterSystem(coordinator);
    PointsSystem::RegisterSystem(coordinator);
    NameSystem::RegisterSystem(coordinator);
    SelectionSystem::RegisterSystem(coordinator);
    CurveControlPointsSystem::RegisterSystem(coordinator);
    C0CurveSystem::RegisterSystem(coordinator);
    C2CurveSystem::RegisterSystem(coordinator);
    ToUpdateSystem::RegisterSystem(coordinator);
    InterpolationCurvesRenderingSystem::RegisterSystem(coordinator);
    InterpolationCurveSystem::RegisterSystem(coordinator);
    C0PatchesSystem::RegisterSystem(coordinator);
    C0PatchesRenderSystem::RegisterSystem(coordinator);
    TrimmedC0PatchesRenderSystem::RegisterSystem(coordinator);
    C2PatchesSystem::RegisterSystem(coordinator);
    C2PatchesRenderSystem::RegisterSystem(coordinator);
    TrimmedC2PatchesRenderSystem::RegisterSystem(coordinator);
    ControlNetSystem::RegisterSystem(coordinator);
    ControlPointsRegistrySystem::RegisterSystem(coordinator);
    GregoryPatchesSystem::RegisterSystem(coordinator);
    VectorSystem::RegisterSystem(coordinator);
    IntersectionSystem::RegisterSystem(coordinator);

    toriSystem = coordinator.GetSystem<ToriSystem>();
    toriRenderingSystem = coordinator.GetSystem<ToriRenderingSystem>();
    trimmedToriRenderingSystem = coordinator.GetSystem<TrimmedToriRenderingSystem>();

    gridSystem = coordinator.GetSystem<GridSystem>();
    cursorSystem = coordinator.GetSystem<CursorSystem>();
    pointsSystem = coordinator.GetSystem<PointsSystem>();
    nameSystem = coordinator.GetSystem<NameSystem>();
    selectionSystem = coordinator.GetSystem<SelectionSystem>();
    c0CurveSystem = coordinator.GetSystem<C0CurveSystem>();
    c2CurveSystem = coordinator.GetSystem<C2CurveSystem>();
    interpolationRenderingSystem = coordinator.GetSystem<InterpolationCurvesRenderingSystem>();

    c0PatchesSystem = coordinator.GetSystem<C0PatchesSystem>();
    c0PatchesRenderSystem = coordinator.GetSystem<C0PatchesRenderSystem>();
    trimmedC0PatchesRenderSystem = coordinator.GetSystem<TrimmedC0PatchesRenderSystem>();

    c2PatchesSystem = coordinator.GetSystem<C2PatchesSystem>();
    c2PatchesRenderSystem = coordinator.GetSystem<C2PatchesRenderSystem>();
    trimmedC2PatchesRenderSystem = coordinator.GetSystem<TrimmedC2PatchesRenderSystem>();

    controlNetSystem = coordinator.GetSystem<ControlNetSystem>();
    controlPointsRegistrySys = coordinator.GetSystem<ControlPointsRegistrySystem>();
    gregoryPatchesSystem = coordinator.GetSystem<GregoryPatchesSystem>();
    vectorSystem = coordinator.GetSystem<VectorSystem>();
    intersectionSystem = coordinator.GetSystem<IntersectionSystem>();

    cameraManager.Init(viewportWidth, viewportHeight);
    gridSystem->Init();
    cursorSystem->Init();
    selectionSystem->Init();
    c0PatchesSystem->Init();
    c2PatchesSystem->Init();
    gregoryPatchesSystem->Init();

    const Entity cursor = cursorSystem->GetCursor();
    nameSystem->SetName(cursor, "Cursor");
    coordinator.AddComponent<Unremovable>(cursor, Unremovable());
}


Entity Modeler::AddTorus()
{
    const Position cursorPos = cursorSystem->GetPosition();

    // Default torus parameters
    constexpr TorusParameters params {
        .majorRadius = 1.0f,
        .minorRadius = 0.2f,
        .meshDensityMinR = 6,
        .meshDensityMajR = 5,
    };

    const Entity entity = toriSystem->AddTorus(cursorPos, params);
    nameSystem->SetName(entity, nameGenerator.GenerateName("Torus_"));

    toriRenderingSystem->AddEntity(entity);

    return entity;
}


Entity Modeler::AddC0Curve(const std::vector<Entity>& controlPoints)
{
    const Entity entity = c0CurveSystem->CreateC0Curve(controlPoints);
    nameSystem->SetName(entity, nameGenerator.GenerateName("CurveC0_"));

    return entity;
}


Entity Modeler::AddC2Curve(const std::vector<Entity>& controlPoints)
{
    const Entity entity = c2CurveSystem->CreateC2Curve(controlPoints);
    nameSystem->SetName(entity, nameGenerator.GenerateName("CurveC2_"));

    return entity;
}


Entity Modeler::AddInterpolationCurve(const std::vector<Entity>& controlPoints)
{
    const auto interpolationSys = coordinator.GetSystem<InterpolationCurveSystem>();
    const Entity entity = interpolationSys->CreateCurve(controlPoints);
    nameSystem->SetName(entity, nameGenerator.GenerateName("InterpolationCurve_"));

    return entity;
}


Entity Modeler::AddC0Plane(const alg::Vec3& direction, const float length, const float width)
{
    const Entity entity = c0PatchesSystem->CreatePlane(cursorSystem->GetPosition(), direction, length, width);
    nameSystem->SetName(entity, nameGenerator.GenerateName("SurfaceC0_"));

    auto const& patches = coordinator.GetComponent<C0Patches>(entity);

    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < patches.PointsInCol(); ++col) {
            const Entity cp = patches.GetPoint(row, col);
            nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
            coordinator.AddComponent(cp, Unremovable());
        }
    }

    coordinator.AddComponent<DrawStd>(entity, DrawStd());

    return entity;
}


Entity Modeler::AddC2Plane(const alg::Vec3& direction, const float length, const float width)
{
    const Entity entity = c2PatchesSystem->CreatePlane(cursorSystem->GetPosition(), direction, length, width);
    nameSystem->SetName(entity, nameGenerator.GenerateName("SurfaceC2_"));

    auto const& patches = coordinator.GetComponent<C2Patches>(entity);

    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < patches.PointsInCol(); ++col) {
            const Entity cp = patches.GetPoint(row, col);
            nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
            coordinator.AddComponent(cp, Unremovable());
        }
    }

    coordinator.AddComponent<DrawStd>(entity, DrawStd());

    return entity;
}


Entity Modeler::AddC0Cylinder()
{
    const Entity entity = c0PatchesSystem->CreateCylinder(cursorSystem->GetPosition(), alg::Vec3(0.f, 1.f, 0.f), 1.f);
    nameSystem->SetName(entity, nameGenerator.GenerateName("CylinderC0_"));

    auto const& patches = coordinator.GetComponent<C0Patches>(entity);

    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < patches.PointsInCol() - 1; ++col) {
            const Entity cp = patches.GetPoint(row, col);
            nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
            coordinator.AddComponent<Unremovable>(cp, Unremovable());
        }
    }

    coordinator.AddComponent<DrawStd>(entity, DrawStd());

    return entity;
}


Entity Modeler::AddC2Cylinder()
{
    const Entity entity = c2PatchesSystem->CreateCylinder(cursorSystem->GetPosition(), alg::Vec3(0.f, 1.f, 0.f), 1.f);
    nameSystem->SetName(entity, nameGenerator.GenerateName("CylinderC2_"));

    auto const& patches = coordinator.GetComponent<C2Patches>(entity);

    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < patches.PointsInCol() - C2PatchesSystem::CylinderDoublePointsCnt; ++col) {
            const Entity cp = patches.GetPoint(row, col);
            nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
            coordinator.AddComponent(cp, Unremovable());
        }
    }

    coordinator.AddComponent<DrawStd>(entity, DrawStd());

    return entity;
}


void Modeler::MergeControlPoints(const Entity e1, const Entity e2)
{
    const auto registrySys = coordinator.GetSystem<ControlPointsRegistrySystem>();
    const auto curveCPSys = coordinator.GetSystem<CurveControlPointsSystem>();

    for (auto owner: registrySys->GetOwnersOfControlPoints(e2)) {
        // Getting system ID
        const auto sysId = std::get<1>(owner);
        const Entity entity = std::get<0>(owner);

        if (sysId == Coordinator::GetSystemID<C0CurveSystem>() ||
            sysId == Coordinator::GetSystemID<C2CurveSystem>() ||
            sysId == Coordinator::GetSystemID<InterpolationCurveSystem>())
            curveCPSys->MergeControlPoints(entity, e2, e1, sysId);

        else if (sysId == Coordinator::GetSystemID<C0PatchesSystem>())
                 c0PatchesSystem->MergeControlPoints(entity, e2, e1, sysId);

        else if (sysId == Coordinator::GetSystemID<C2PatchesSystem>())
                 c2PatchesSystem->MergeControlPoints(entity, e2, e1);

        else
            throw std::runtime_error("Unknown system");
    }

    auto const& pos1 = coordinator.GetComponent<Position>(e1);
    auto const& pos2 = coordinator.GetComponent<Position>(e2);
    const Position newPos((pos1.vec + pos2.vec) * 0.5f);

    coordinator.SetComponent<Position>(e1, newPos);
    coordinator.DestroyEntity(e2);
}


void Modeler::AddRowOfC0PlanePatches(const Entity surface, const alg::Vec3 &direction, const float length, const float width)
{
    c0PatchesSystem->AddRowOfPlanePatches(surface, cursorSystem->GetPosition(), direction, length, width);

    auto const& patches = coordinator.GetComponent<C0Patches>(surface);

    for (int col=0; col < patches.PointsInCol(); col++) {
        for (int row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
            const Entity cp = patches.GetPoint(row, col);

            nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
            coordinator.AddComponent(cp, Unremovable());
        }
    }
}


void Modeler::AddColOfC0PlanePatches(const Entity surface, const alg::Vec3 &direction, float length, float width)
{
    c0PatchesSystem->AddColOfPlanePatches(surface, cursorSystem->GetPosition(), direction, length, width);

    auto const& patches = coordinator.GetComponent<C0Patches>(surface);

    for (int row=0; row < patches.PointsInRow(); row++) {
        for (int col=patches.PointsInCol() - 3; col < patches.PointsInCol(); col++) {
            Entity cp = patches.GetPoint(row, col);

            nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
        }
    }
}


void Modeler::AddRowOfC2PlanePatches(const Entity surface, const alg::Vec3 &direction, const float length, const float width)
{
    c2PatchesSystem->AddRowOfPlanePatches(surface, cursorSystem->GetPosition(), direction, length, width);

    auto const& patches = coordinator.GetComponent<C2Patches>(surface);

    for (int col=0; col < patches.PointsInCol(); col++) {
        const Entity cp = patches.GetPoint(patches.PointsInRow() - 1, col);

        nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
        
    }
}


void Modeler::AddColOfC2PlanePatches(const Entity surface, const alg::Vec3 &direction, const float length, const float width)
{
    c2PatchesSystem->AddColOfPlanePatches(surface, cursorSystem->GetPosition(), direction, length, width);

    auto const& patches = coordinator.GetComponent<C2Patches>(surface);

    for (int row=0; row < patches.PointsInRow(); row++) {
        const Entity cp = patches.GetPoint(row, patches.PointsInCol()-1);

        nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
        coordinator.AddComponent(cp, Unremovable());
    }
}


void Modeler::AddRowOfC0CylinderPatches(const Entity cylinder, const float radius, const alg::Vec3 &dir)
{
    c0PatchesSystem->AddRowOfCylinderPatches(cylinder, cursorSystem->GetPosition(), dir, radius);

    auto const& patches = coordinator.GetComponent<C0Patches>(cylinder);

    for (int col=0; col < patches.PointsInCol() - 1; col++) {
        for (int row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
            const Entity cp = patches.GetPoint(row, col);

            nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
            coordinator.AddComponent(cp, Unremovable());
        }
    }
}


void Modeler::AddColOfC0CylinderPatches(Entity surface, float radius, const alg::Vec3 &dir)
{
    c0PatchesSystem->AddColOfCylinderPatches(surface, cursorSystem->GetPosition(), dir, radius);

    auto const& patches = coordinator.GetComponent<C0Patches>(surface);

    for (int row=0; row < patches.PointsInRow(); row++) {
        for (int col=patches.PointsInCol() - 4; col < patches.PointsInCol() - 1; col++) {
            Entity cp = patches.GetPoint(row, col);

            nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
            coordinator.AddComponent(cp, Unremovable());
        }
    }
}


void Modeler::AddRowOfC2CylinderPatches(const Entity surface, const float radius, const alg::Vec3 &dir)
{
    c2PatchesSystem->AddRowOfCylinderPatches(surface, cursorSystem->GetPosition(), dir, radius);

    auto const& patches = coordinator.GetComponent<C2Patches>(surface);

    for (int col=0; col < patches.PointsInCol() - C2PatchesSystem::CylinderDoublePointsCnt; col++) {
        Entity cp = patches.GetPoint(patches.PointsInRow()-1, col);
        
        nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
        coordinator.AddComponent(cp, Unremovable());
    }
}


void Modeler::AddColOfC2CylinderPatches(const Entity surface, const float radius, const alg::Vec3 &dir)
{
    c2PatchesSystem->AddColOfCylinderPatches(surface, cursorSystem->GetPosition(), dir, radius);

    auto const& patches = coordinator.GetComponent<C2Patches>(surface);

    for (int row=0; row < patches.PointsInRow(); row++) {
        const Entity cp = patches.GetPoint(row, patches.PointsInCol()-C2PatchesSystem::CylinderDoublePointsCnt-1);

        nameSystem->SetName(cp, nameGenerator.GenerateName("Point_"));
        coordinator.AddComponent(cp, Unremovable());
    }
}


Entity Modeler::Add3DPointFromViewport(const float x, const float y)
{
    const Position newPos(PointFromViewportCoordinates(x, y));
    const Entity result = pointsSystem->CreatePoint(newPos);
    nameSystem->SetName(result, nameGenerator.GenerateName("Point_"));

    return result;
}


void Modeler::TryToSelectFromViewport(const float x, const float y)
{
    const Line line(LineFromViewportCoordinates(x, y));
    selectionSystem->SelectFromLine(line);
}


void Modeler::SelectMultipleFromViewport(const float x, const float y, const float dist)
{
    const Line line(LineFromViewportCoordinates(x, y));
    selectionSystem->SelectAllFromLine(line, dist);
}


std::vector<GregoryPatchesSystem::Hole> Modeler::GetHolesPossibleToFill(const std::unordered_set<Entity> &entities) const
{
    std::vector<C0Patches> c0Patches;
    c0Patches.reserve(entities.size());

    for (const auto entity: entities)
        c0Patches.push_back(coordinator.GetComponent<C0Patches>(entity));

    return gregoryPatchesSystem->FindHolesToFill(c0Patches);
}


Entity Modeler::FillHole(const GregoryPatchesSystem::Hole& hole)
{
    const Entity entity = gregoryPatchesSystem->FillHole(hole);
    nameSystem->SetName(entity, nameGenerator.GenerateName("GregoryPatches_"));

    return entity;
}


std::optional<Entity> Modeler::FindIntersection(const Entity e1, const Entity e2, const float step)
{
    auto result = intersectionSystem->FindIntersection(e1, e2, step);
    if (!result.has_value())
        return std::nullopt;

    SetIntersectionCurveUp(result.value(), e1, e2);

    return result.value();
}


std::optional<Entity> Modeler::FindIntersection(const Entity e1, const Entity e2, const float step, const Position &guidance)
{
    const auto result = intersectionSystem->FindIntersection(e1, e2, step, guidance);
    if (!result.has_value())
        return std::nullopt;

    SetIntersectionCurveUp(result.value(), e1, e2);

    return result.value();
}


std::optional<Entity> Modeler::FindSelfIntersection(const Entity e, const float step)
{
    const auto result = intersectionSystem->FindSelfIntersection(e, step);
    if (!result.has_value())
        return std::nullopt;

    SetIntersectionCurveUp(result.value(), e, e);

    return result.value();
}


std::optional<Entity> Modeler::FindSelfIntersection(const Entity e,const float step, const Position &guidance)
{
    const auto result = intersectionSystem->FindSelfIntersection(e, step, guidance);
    if (!result.has_value())
        return std::nullopt;

    SetIntersectionCurveUp(result.value(), e, e);

    return result.value();
}


Entity Modeler::TurnIntersectionCurveToInterpolation(const Entity curve)
{
    const auto& oldCps = coordinator.GetComponent<CurveControlPoints>(curve);
    std::vector<Entity> newCps;
    newCps.reserve(oldCps.Size());

    for (const auto oldCp: oldCps.GetPoints()) {
        const auto& pos = coordinator.GetComponent<Position>(oldCp);
        Entity newCp = pointsSystem->CreatePoint(pos);

        newCps.push_back(newCp);
        nameSystem->SetName(newCp, nameGenerator.GenerateName("Point_"));
    }

    coordinator.DestroyEntity(curve);

    const Entity newCurve = coordinator.GetSystem<InterpolationCurveSystem>()->CreateCurve(newCps);
    nameSystem->SetName(newCurve, nameGenerator.GenerateName("InterpolationCurve_"));

    return newCurve;
}


void Modeler::ClearScene()
{
    const std::vector<std::shared_ptr<System>> systemsToClear {
        intersectionSystem,
        vectorSystem,
        gregoryPatchesSystem,
        c2PatchesSystem,
        c0PatchesSystem,
        coordinator.GetSystem<InterpolationCurveSystem>(),
        c2CurveSystem,
        c0CurveSystem,
        pointsSystem,
        toriSystem
    };

    std::stack<Entity> toDelete;

    for (const auto& system: systemsToClear) {
        for (auto entity: system->GetEntities()) {
            toDelete.push(entity);
        }

        while (!toDelete.empty()) {
            coordinator.DestroyEntity(toDelete.top());
            toDelete.pop();
        }
    }
}


void Modeler::FillTrimmingRegion(const Entity e, const size_t u, const size_t v)
{
    UvVisualizer visualizer(coordinator);
    visualizer.FillTrimmingRegion(e, u, v);
}


void Modeler::DrawPointOnUV(const Entity e, const size_t u, const size_t v)
{
    UvVisualizer visualizer(coordinator);
    visualizer.DrawPoint(e, u, v);
}


void Modeler::ApplyTrimming(const Entity e)
{
    if (toriRenderingSystem->HasEntity(e)) {
        toriRenderingSystem->RemoveEntity(e);
        trimmedToriRenderingSystem->AddEntity(e);
    }

    if (coordinator.HasComponent<DrawStd>(e))
        coordinator.DeleteComponent<DrawStd>(e);

    coordinator.AddComponent<DrawTrimmed>(e, DrawTrimmed());
}


void Modeler::ShowDerivativesU(Entity e)
{
    constexpr int cnt = 10;

    const auto& patches = coordinator.GetComponent<C2Patches>(e);
    const float maxU = C2PatchesSystem::MaxU(patches);
    const float maxV = C2PatchesSystem::MaxV(patches);

    for (int row = 0; row < cnt; row++) {
        for (int col = 0; col < cnt; col++) {
            const float u = static_cast<float>(row) * maxU / static_cast<float>(cnt);
            const float v = static_cast<float>(col) * maxV / static_cast<float>(cnt);

            auto point = c2PatchesSystem->PointOnSurface(patches, u, v);
            auto partialU = c2PatchesSystem->PartialDerivativeU(patches, u, v);

            vectorSystem->AddVector(partialU.Normalize(), point);
        }
    }
}


void Modeler::ShowDerivativesV(Entity e)
{
    constexpr int cnt = 10;

    const auto& patches = coordinator.GetComponent<C2Patches>(e);
    const float maxU = C2PatchesSystem::MaxU(patches);
    const float maxV = C2PatchesSystem::MaxV(patches);

    for (int row = 0; row < cnt; row++) {
        for (int col = 0; col < cnt; col++) {
            const float u = static_cast<float>(row) * maxU / static_cast<float>(cnt);
            const float v = static_cast<float>(col) * maxV / static_cast<float>(cnt);

            auto point = c2PatchesSystem->PointOnSurface(patches, u, v);
            auto partialU = c2PatchesSystem->PartialDerivativeV(patches, u, v);

            vectorSystem->AddVector(partialU.Normalize(), point);
        }
    }
}


void Modeler::ShowC2Normals(const Entity e)
{
    constexpr int cntRows = 10;
    constexpr int cntCols = 10;

    const auto& patches = coordinator.GetComponent<C2Patches>(e);
    const float maxU = C2PatchesSystem::MaxU(patches);
    const float maxV = C2PatchesSystem::MaxV(patches);

    for (int row = 0; row <= cntRows; row++) {
        for (int col = 0; col <= cntCols; col++) {
            const float u = static_cast<float>(row) * maxU / static_cast<float>(cntRows);
            const float v = static_cast<float>(col) * maxV / static_cast<float>(cntCols);

            auto point = c2PatchesSystem->PointOnSurface(patches, u, v);
            auto partialV = c2PatchesSystem->PartialDerivativeV(patches, u, v);
            auto partialU = c2PatchesSystem->PartialDerivativeU(patches, u, v);

            auto normal = alg::Cross(partialV, partialU).Normalize();

            vectorSystem->AddVector(normal, point);
        }
    }
}

void Modeler::ShowC2Normals(Entity e, float u, float v)
{
    auto point = c2PatchesSystem->PointOnSurface(e, u, v);
    auto partialV = c2PatchesSystem->PartialDerivativeV(e, u, v);
    auto partialU = c2PatchesSystem->PartialDerivativeU(e, u, v);

    auto normal = alg::Cross(partialV, partialU).Normalize();

    vectorSystem->AddVector(normal, point);
}


void Modeler::ShowC0Normals(Entity e)
{
    constexpr int cntRows = 0;
    constexpr int cntCols = 30;

    const auto& patches = coordinator.GetComponent<C0Patches>(e);
    const float maxU = C0PatchesSystem::MaxU(patches);
    const float maxV = C0PatchesSystem::MaxV(patches);

    for (int row = 0; row <= cntRows; row++) {
        for (int col = 0; col <= cntCols; col++) {
            const float u = 0; //static_cast<float>(row) * maxU / static_cast<float>(cntRows);
            const float v = static_cast<float>(col) * maxV / static_cast<float>(cntCols);

            auto point = c0PatchesSystem->PointOnSurface(patches, u, v);
            auto partialV = c0PatchesSystem->PartialDerivativeV(patches, u, v);
            auto partialU = c0PatchesSystem->PartialDerivativeU(patches, u, v);

            auto normal = alg::Cross(partialU, partialV).Normalize();

            vectorSystem->AddVector(normal, point);
        }
    }
}


void Modeler::ShowC0Normals(Entity e, float u, float v)
{
    auto point = c0PatchesSystem->PointOnSurface(e, u, v);
    auto partialV = c0PatchesSystem->PartialDerivativeV(e, u, v);
    auto partialU = c0PatchesSystem->PartialDerivativeU(e, u, v);

    auto normal = alg::Cross(partialU, partialV).Normalize();

    vectorSystem->AddVector(normal, point);
}


void Modeler::Update() const
{
    c0CurveSystem->Update();
    c0PatchesSystem->Update();
    c2CurveSystem->Update();
    c2PatchesSystem->Update();
    gregoryPatchesSystem->Update();
    interpolationRenderingSystem->Update();
}


alg::Vec3 Modeler::PointFromViewportCoordinates(const float x, const float y)
{
    const auto cameraParams = cameraManager.GetBaseParams();
    auto const& cameraTarget = cameraParams.target;
    auto const& cameraPos = cameraManager.GetCameraPosition();
    const auto cursorPos = cursorSystem->GetPosition();

    const Line nearToFar = LineFromViewportCoordinates(x, y);

    const alg::Vec3 cameraDirection = cameraTarget.vec - cameraPos.vec;
    const Plane perpendicularToScreenWithCursor(cursorPos.vec, cameraDirection);

    std::optional<alg::Vec3> intersection =
        perpendicularToScreenWithCursor.Intersect(nearToFar);

    if (!intersection.has_value()) {
        throw std::runtime_error("Cannot project viewport coordinates to 3d ones");
    }

    return intersection.value();
}


Line Modeler::LineFromViewportCoordinates(const float x, const float y)
{
    const auto cameraType = cameraManager.GetCurrentCameraType();
    if (cameraType == CameraManager::CameraType::Anaglyphs)
        cameraManager.SetCameraType(CameraManager::CameraType::Perspective);

    const auto viewMtx = cameraManager.ViewMtx();
    const auto perspectiveMtx = cameraManager.PerspectiveMtx();

    cameraManager.SetCameraType(cameraType);

    const auto cameraInv = (perspectiveMtx * viewMtx).Inverse().value();

    alg::Vec4 nearV4 = cameraInv * alg::Vec4(x, y, -1.0f, 1.0f);
    alg::Vec4 farV4 = cameraInv * alg::Vec4(x, y, 1.0f, 1.0f);

    const auto near = alg::Vec3(
        nearV4.X() / nearV4.W(),
        nearV4.Y() / nearV4.W(),
        nearV4.Z() / nearV4.W()
    );

    const auto far = alg::Vec3(
        farV4.X() / farV4.W(),
        farV4.Y() / farV4.W(),
        farV4.Z() / farV4.W()
    );

    return Line::FromTwoPoints(near, far);
}

void Modeler::SetIntersectionCurveUp(const Entity curve, const  Entity e1, const Entity e2)
{
    nameSystem->SetName(curve, nameGenerator.GenerateName("IntersectionCurve_"));

    UvVisualizer visualizer(coordinator);

    const auto& interCurve = coordinator.GetComponent<IntersectionCurve>(curve);

    if (e1 == e2) {
        visualizer.VisualizeLineOnParameters(e1, interCurve, UvVisualizer::SurfaceCurveRelation::Both);
        return;
    }

    visualizer.VisualizeLineOnParameters(e1, interCurve, UvVisualizer::SurfaceCurveRelation::Surface1);
    visualizer.VisualizeLineOnParameters(e2, interCurve, UvVisualizer::SurfaceCurveRelation::Surface2);
}


void Modeler::RenderSystemsObjects(
    const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, const float nearPlane, const float farPlane
) const {
    const alg::Mat4x4 cameraMtx = persMtx * viewMtx;

    toriRenderingSystem->Render(cameraMtx);
    trimmedToriRenderingSystem->Render(cameraMtx);

    cursorSystem->Render(cameraMtx);
    pointsSystem->Render(cameraMtx);
    selectionSystem->RenderMiddlePoint(cameraMtx);
    c0CurveSystem->Render(cameraMtx);
    c2CurveSystem->Render(cameraMtx);
    interpolationRenderingSystem->Render(cameraMtx);

    c0PatchesRenderSystem->Render(cameraMtx);
    trimmedC0PatchesRenderSystem->Render(cameraMtx);

    c2PatchesRenderSystem->Render(cameraMtx);
    trimmedC2PatchesRenderSystem->Render(cameraMtx);

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
