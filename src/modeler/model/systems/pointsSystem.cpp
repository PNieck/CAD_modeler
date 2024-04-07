#include <CAD_modeler/model/systems/pointsSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <CAD_modeler/model/components/name.hpp>
#include <CAD_modeler/model/components/position.hpp>


void PointsSystem::RegisterSystem(Coordinator& coordinator)
{
    coordinator.RegisterSystem<PointsSystem>();

    coordinator.RegisterComponent<Position>();
    coordinator.RegisterComponent<Name>();
}


PointsSystem::PointsSystem()
{
    std::vector<float> vertices = {
        0.0f, 0.0f, 0.0f
    };

    std::vector<uint32_t> indices = { 0 };

    pointsMesh.Update(vertices, indices);
}


Entity PointsSystem::CreatePoint(Position& pos)
{
    Entity newPoint = coordinator->CreateEntity();

    coordinator->AddComponent<Position>(newPoint, pos);
    coordinator->AddComponent<Name>(newPoint, nameGenerator.GenerateName("Point"));

    entities.insert(newPoint);

    return newPoint;
}


void PointsSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    glm::mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(glm::vec4(1.0f));

    for (auto const entity : entities) {
        auto const& position = coordinator->GetComponent<Position>(entity);

        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

        shader.SetMVP(cameraMtx * position.TranslationMatrix());
        
        pointsMesh.Use();
        glDrawElements(GL_POINTS, pointsMesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(glm::vec4(1.0f));
    }
}