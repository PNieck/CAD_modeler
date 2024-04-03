#include <CAD_modeler/model/systems/pointsSystem.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>

#include <CAD_modeler/model/components/name.hpp>

#include <CAD_modeler/model/ecsCoordinator.hpp>


void PointsSystem::RegisterSystem(Coordinator& coordinator)
{
    coordinator.RegisterSystem<PointsSystem>();

    coordinator.RegisterComponent<Position>();
    coordinator.RegisterComponent<Name>();
}


PointsSystem::PointsSystem():
    shader("../../shaders/vertexShader.vert", "../../shaders/fragmentShader.frag")
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

    shader.use();
    shader.setVec4("color", glm::vec4(1.0f));

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    glm::mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    for (auto const entity : entities) {
        auto const& position = coordinator->GetComponent<Position>(entity);

        shader.setMatrix4("MVP", cameraMtx * position.TranslationMatrix());
        
        pointsMesh.Use();
        glDrawElements(GL_POINTS, pointsMesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
    }
}
