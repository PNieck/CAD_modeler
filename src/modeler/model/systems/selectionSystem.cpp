#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/selected.hpp>
#include <CAD_modeler/model/components/scale.hpp>
#include <CAD_modeler/model/components/rotation.hpp>

#include <CAD_modeler/model/systems/pointsSystem.hpp>
#include <CAD_modeler/model/systems/toriSystem.hpp>
#include <CAD_modeler/model/systems/cameraSystem.hpp>

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


void SelectionSystem::Init(ShaderRepository* shaderRepo)
{
    this->shadersRepo = shaderRepo;

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

    float smallestT = std::numeric_limits<float>::infinity();
    Entity actEntity;

    for (auto const entity: pointsSystem->GetEntities()) {
        auto const& position = coordinator->GetComponent<Position>(entity);

        float t = glm::dot(line.GetDirection(), position.vec - line.GetSamplePoint());
        glm::vec3 projection = line.GetPointOnLine(t);

        // TODO: change to distance Squared
        float distance = glm::distance(projection, position.vec);

        if (t < smallestT && distance < 0.5) {
            smallestT = t;
            actEntity = entity;
        }
    }

    if (smallestT != std::numeric_limits<float>::infinity()) {
        if (IsSelected(actEntity))
            Deselect(actEntity);
        else
            Select(actEntity);
    }
}


void SelectionSystem::RenderMiddlePoint()
{
    if (entities.size() < 2)
        return;

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shadersRepo->GetStdShader();

    glm::mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    auto const& mesh = coordinator->GetComponent<Mesh>(middlePoint);
    auto const& position = coordinator->GetComponent<Position>(middlePoint);

    shader.Use();

    shader.SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    glm::mat4x4 modelMtx = position.TranslationMatrix();
    shader.SetMVP(cameraMtx * modelMtx);

    mesh.Use();
    glDrawElements(GL_POINTS, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
}


void SelectionSystem::UpdateMiddlePointPosition()
{
    Position newPos(0.0f);
    int cnt = 0;

    for (auto entity: entities) {
        if (coordinator->GetEntityComponents(entity).contains(ComponentsManager::GetComponentId<Position>())) {
            newPos.vec += coordinator->GetComponent<Position>(entity).vec;
            cnt++;
        }
    }

    if (cnt != 0)
        newPos.vec /= cnt;

    coordinator->SetComponent<Position>(middlePoint, newPos);
}


void SelectionSystem::MoveSelected(const Position & newMiddlePointPos)
{
    Entity midPoint = GetMiddlePoint();
    Position const& midPointPos = coordinator->GetComponent<Position>(midPoint);

    glm::vec3 delta = newMiddlePointPos.vec - midPointPos.vec;

    for (auto entity: entities) {
        if (!coordinator->GetEntityComponents(entity).contains(ComponentsManager::GetComponentId<Position>()))
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
            glm::vec3 dist = pos.vec - midPointPos.vec;

            dist.x *= scale.GetX();
            dist.y *= scale.GetY();
            dist.z *= scale.GetZ();

            Position newPos(midPointPos.vec + dist);
            coordinator->SetComponent<Position>(entity, newPos);
        }

        if (components.contains(ComponentsManager::GetComponentId<Scale>())) {
            auto const& sc = coordinator->GetComponent<Scale>(entity);
            Scale newScale(sc.scale * scale.scale);
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
            glm::vec3 dist = pos.vec - midPointPos.vec;

            dist = glm::rotate(rotation.quat, dist);

            Position newPos(midPointPos.vec + dist);
            coordinator->SetComponent<Position>(entity, newPos);
        }

        if (components.contains(ComponentsManager::GetComponentId<Rotation>())) {
            auto& rot = coordinator->GetComponent<Rotation>(entity);
            auto quat = rotation.quat * rot.quat;
            quat = glm::normalize(quat);

            coordinator->SetComponent<Rotation>(entity, Rotation(quat));
        }
    }
}
