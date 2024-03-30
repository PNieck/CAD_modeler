#include <CAD_modeler/model/systems/meshRenderer.hpp>

#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/scale.hpp>
#include <CAD_modeler/model/components/rotation.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>

#include <CAD_modeler/model/ecsCoordinator.hpp>


void MeshRenderer::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<MeshRenderer>();

    coordinator.RegisterComponent<Mesh>();
    coordinator.RegisterComponent<Position>();
    coordinator.RegisterComponent<Scale>();
    coordinator.RegisterComponent<Rotation>();

    coordinator.RegisterRequiredComponent<MeshRenderer, Mesh>();
    coordinator.RegisterRequiredComponent<MeshRenderer, Position>();
    coordinator.RegisterRequiredComponent<MeshRenderer, Scale>();
    coordinator.RegisterRequiredComponent<MeshRenderer, Rotation>();
}


MeshRenderer::MeshRenderer():
    shader("../../shaders/vertexShader.vert", "../../shaders/fragmentShader.frag")
{}


void MeshRenderer::Render()
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    glm::mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        auto const& position = coordinator->GetComponent<Position>(entity);
        auto const& scale = coordinator->GetComponent<Scale>(entity);
        auto const& rotation = coordinator->GetComponent<Rotation>(entity);

        shader.use();

        glm::mat4x4 modelMtx = scale.ScaleMatrix() * rotation.GetRotationMatrix() * position.TranslationMatrix();
        shader.setMatrix4("MVP", cameraMtx * modelMtx);
        
        mesh.Use();
        glDrawElements(GL_LINE_LOOP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
