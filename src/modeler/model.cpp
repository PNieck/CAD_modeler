#include <CAD_modeler/model.hpp>

#include <CAD_modeler/utilities/line.hpp>
#include <CAD_modeler/utilities/plane.hpp>

#include <CAD_modeler/model/components/registerComponents.hpp>

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

    cameraSys = coordinator.GetSystem<CameraSystem>();
    toriSystem = coordinator.GetSystem<ToriSystem>();
    gridSystem = coordinator.GetSystem<GridSystem>();
    cursorSystem = coordinator.GetSystem<CursorSystem>();
    pointsSystem = coordinator.GetSystem<PointsSystem>();
    nameSystem = coordinator.GetSystem<NameSystem>();
    selectionSystem = coordinator.GetSystem<SelectionSystem>();

    cameraSys->Init(viewport_width, viewport_height);
    gridSystem->Init(&shadersRepo);
    cursorSystem->Init(&shadersRepo);
    selectionSystem->Init(&shadersRepo);
    pointsSystem->Init(&shadersRepo);
    toriSystem->Init(&shadersRepo);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}


void Model::RenderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);

    gridSystem->Render();
    toriSystem->Render();
    cursorSystem->Render();
    pointsSystem->Render();
    selectionSystem->RenderMiddlePoint();
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


void Model::ChangeSelectedEntitiesPosition(const Position& newMidPoint)
{
    Entity midPoint = selectionSystem->GetMiddlePoint();
    Position const& midPointPos = coordinator.GetComponent<Position>(midPoint);

    glm::vec3 translation = newMidPoint.vec - midPointPos.vec;

    auto const& selected = selectionSystem->SelectedEntities();

    for (auto entity: selected) {
        coordinator.GetComponent<Position>(entity).vec += translation;
    }
}


void Model::ChangeSelectedEntitiesScale(const Scale& scale)
{
    Entity midPoint = selectionSystem->GetMiddlePoint();
    Position const& midPointPos = coordinator.GetComponent<Position>(midPoint);

    auto const& selected = selectionSystem->SelectedEntities();

    for (auto entity: selected) {
        Position& pos = coordinator.GetComponent<Position>(entity);
        glm::vec3 dist = pos.vec - midPointPos.vec;

        dist.x *= scale.GetX();
        dist.y *= scale.GetY();
        dist.z *= scale.GetZ();

        pos.vec = midPointPos.vec + dist;

        if (coordinator.GetEntityComponents(entity).contains(ComponentsManager::GetComponentId<Scale>())) {
            auto& sc = coordinator.GetComponent<Scale>(entity);
            sc.scale *= scale.scale;
        }
    }
}


void Model::RotateSelectedEntities(const Rotation& rotation)
{
    Entity midPoint = selectionSystem->GetMiddlePoint();
    Position const& midPointPos = coordinator.GetComponent<Position>(midPoint);

    auto const& selected = selectionSystem->SelectedEntities();

    for (auto entity: selected) {
        Position& pos = coordinator.GetComponent<Position>(entity);
        glm::vec3 dist = pos.vec - midPointPos.vec;

        dist = glm::rotate(rotation.quat, dist);

        pos.vec = midPointPos.vec + dist;

        if (coordinator.GetEntityComponents(entity).contains(ComponentsManager::GetComponentId<Rotation>())) {
            auto& rot = coordinator.GetComponent<Rotation>(entity);
            auto quat = rotation.quat * rot.quat;
            quat = glm::normalize(quat);

            coordinator.GetComponent<Rotation>(entity) = Rotation(quat);
        }
    }
}
