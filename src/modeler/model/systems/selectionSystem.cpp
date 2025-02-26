#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/selected.hpp>
#include <CAD_modeler/model/components/scale.hpp>
#include <CAD_modeler/model/components/rotation.hpp>

#include <CAD_modeler/model/systems/pointsSystem.hpp>
#include <CAD_modeler/model/systems/toriSystem.hpp>
#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>

#include <CAD_modeler/utilities/angle.hpp>

#include <stdexcept>


void SelectionSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<SelectionSystem>();

    coordinator.RegisterRequiredComponent<SelectionSystem, Selected>();
}


SelectionSystem::SelectionSystem()
{
    std::vector<float> vertices = {
        0.0f, 0.0f, 0.0f
    };

    std::vector<uint32_t> indices = { 0 };

    pointsMesh.Update(vertices, indices);
}


void SelectionSystem::Init()
{
    middlePoint = coordinator->CreateEntity();

    Position pos;
    coordinator->AddComponent<Position>(middlePoint, pos);

    coordinator->AddComponent<Mesh>(middlePoint, pointsMesh);
}


void SelectionSystem::Select(Entity entity)
{
    coordinator->AddComponent<Selected>(entity, Selected());

    UpdateMiddlePointPosition();
}


void SelectionSystem::Deselect(Entity entity)
{
    coordinator->DeleteComponent<Selected>(entity);

    UpdateMiddlePointPosition();
}


void SelectionSystem::DeselectAllEntities()
{
    auto it = entities.begin();
    
    // Cannot use standard for, because collection changes during iteration
    while (it != entities.end()) {
        coordinator->DeleteComponent<Selected>(*(it++));
    }
}


void SelectionSystem::SelectFromLine(const Line& line)
{
    auto const& pointsSystem = coordinator->GetSystem<PointsSystem>();

    float smallestDis = std::numeric_limits<float>::infinity();
    Entity actEntity = 0;

    for (auto const entity: pointsSystem->GetEntities()) {
        auto const& position = coordinator->GetComponent<Position>(entity);

        float t = alg::Dot(line.GetDirection(), position.vec - line.GetSamplePoint());
        alg::Vec3 projection = line.GetPointOnLine(t);

        float dis = alg::DistanceSquared(projection, position.vec);

        if (dis < smallestDis && dis < 0.5) {
            smallestDis = dis;
            actEntity = entity;
        }
    }

    if (smallestDis != std::numeric_limits<float>::infinity()) {
        if (IsSelected(actEntity))
            Deselect(actEntity);
        else
            Select(actEntity);
    }
}


void SelectionSystem::SelectAllFromLine(const Line &line, float maxDist)
{
    auto const& pointsSystem = coordinator->GetSystem<PointsSystem>();

    for (auto const entity: pointsSystem->GetEntities()) {
        auto const& position = coordinator->GetComponent<Position>(entity);

        float t = alg::Dot(line.GetDirection(), position.vec - line.GetSamplePoint());
        alg::Vec3 projection = line.GetPointOnLine(t);

        float dis = alg::Distance(projection, position.vec);

        if (dis <= maxDist)
            Select(entity);
    }
}


void SelectionSystem::RenderMiddlePoint(const alg::Mat4x4& cameraMtx)
{
    if (entities.size() < 2)
        return;

    auto const& shader = ShaderRepository::GetInstance().GetStdShader();

    auto const& mesh = coordinator->GetComponent<Mesh>(middlePoint);
    auto const& position = coordinator->GetComponent<Position>(middlePoint);

    shader.Use();

    shader.SetColor(alg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));

    alg::Mat4x4 modelMtx = position.TranslationMatrix();
    shader.SetMVP(cameraMtx * modelMtx);

    mesh.Use();
    glDrawElements(GL_POINTS, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
}


void SelectionSystem::UpdateMiddlePointPosition()
{
    Position newPos(0.0f);
    int cnt = 0;

    for (auto entity: entities) {
        if (coordinator->HasComponent<Position>(entity)) {
            newPos.vec += coordinator->GetComponent<Position>(entity).vec;
            cnt++;
        }
    }

    if (cnt != 0)
        newPos.vec /= static_cast<float>(cnt);

    coordinator->SetComponent<Position>(middlePoint, newPos);
}


void SelectionSystem::MoveSelected(const Position & newMiddlePointPos)
{
    Entity midPoint = GetMiddlePoint();
    Position const& midPointPos = coordinator->GetComponent<Position>(midPoint);

    alg::Vec3 delta = newMiddlePointPos.vec - midPointPos.vec;

    for (auto entity: entities) {
        if (!coordinator->HasComponent<Position>(entity))
            continue;

        auto const& oldPos = coordinator->GetComponent<Position>(entity);

        Position newPos(oldPos.vec + delta);
        coordinator->SetComponent<Position>(entity, newPos);
    }

    coordinator->SetComponent<Position>(middlePoint, newMiddlePointPos);
}


void SelectionSystem::ScaleSelected(const Scale& scale)
{
    Entity midPoint = GetMiddlePoint();
    Position const& midPointPos = coordinator->GetComponent<Position>(midPoint);

    for (auto entity: entities) {
        auto const& components = coordinator->GetEntityComponents(entity);

        if (components.contains(ComponentsManager::GetComponentId<Position>())) {
            Position const& pos = coordinator->GetComponent<Position>(entity);
            alg::Vec3 dist = pos.vec - midPointPos.vec;

            dist.X() *= scale.GetX();
            dist.Y() *= scale.GetY();
            dist.Z() *= scale.GetZ();

            Position newPos(midPointPos.vec + dist);
            coordinator->SetComponent<Position>(entity, newPos);
        }

        if (components.contains(ComponentsManager::GetComponentId<Scale>())) {
            auto const& sc = coordinator->GetComponent<Scale>(entity);

            Scale newScale(
                sc.GetX() * scale.GetX(),
                sc.GetY() * scale.GetY(),
                sc.GetZ() * scale.GetZ()   
            );

            coordinator->SetComponent<Scale>(entity, newScale);
        }
    }
}


void SelectionSystem::RotateSelected(const Rotation & rotation)
{
    Entity midPoint = GetMiddlePoint();
    Position const& midPointPos = coordinator->GetComponent<Position>(midPoint);

    for (auto entity: entities) {
        auto const& components = coordinator->GetEntityComponents(entity);

        if (components.contains(ComponentsManager::GetComponentId<Position>())) {
            Position const& pos = coordinator->GetComponent<Position>(entity);
            alg::Vec3 dist = pos.vec - midPointPos.vec;

            dist = rotation.GetQuaternion().Rotate(dist);

            Position newPos(midPointPos.vec + dist);
            coordinator->SetComponent<Position>(entity, newPos);
        }

        if (components.contains(ComponentsManager::GetComponentId<Rotation>())) {
            auto& rot = coordinator->GetComponent<Rotation>(entity);
            auto quat = rotation.GetQuaternion() * rot.GetQuaternion();
            quat = quat.Normalize();

            coordinator->SetComponent<Rotation>(entity, Rotation(quat));
        }
    }
}


void SelectionSystem::RenderSelectionCircle(float x, float y)
{
    circleMesh.Update(
        GenerateSelectionCircleVertices(x, y, 0.05f),
        GenerateSelectionCircleIndices()
    );

    auto const& shader = ShaderRepository::GetInstance().GetPassThroughShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));

    glDrawElements(GL_LINE_LOOP, circleMesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
}


std::vector<float> SelectionSystem::GenerateSelectionCircleVertices(float x, float y, float radius)
{
    alg::Vec3 center(x, y, 0.0f);
    std::vector<float> result;
    result.reserve(20 * 3);

    for (int i=0; i < 20; i++) {
        alg::Vec3 v(0.0f, radius, 0.0f);

        Angle angle = Angle::FromDegrees((360.f / 20.f) * i);
        Rotation rot(alg::Vec3(0.0f, 0.0f, 1.0f), angle.ToRadians());

        rot.Rotate(v);

        alg::Vec3 point = center + v;

        result.push_back(point.X());
        result.push_back(point.Y());
        result.push_back(point.Z());
    }

    return result;
}


std::vector<uint32_t> SelectionSystem::GenerateSelectionCircleIndices()
{
    std::vector<uint32_t> result(20);

    for (int i=0; i < 20; i++)
        result[i] = i;

    return result;
}
