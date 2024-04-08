#include <CAD_modeler/model/systems/toriSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/scale.hpp>
#include <CAD_modeler/model/components/rotation.hpp>
#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/name.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <vector>


void ToriSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<ToriSystem>();

    coordinator.RegisterRequiredComponent<ToriSystem, Position>();
    coordinator.RegisterRequiredComponent<ToriSystem, Scale>();
    coordinator.RegisterRequiredComponent<ToriSystem, Rotation>();
    coordinator.RegisterRequiredComponent<ToriSystem, Mesh>();
    coordinator.RegisterRequiredComponent<ToriSystem, TorusParameters>();
}


void ToriSystem::Init(ShaderRepository * shaderRepo)
{
    this->shadersRepo = shaderRepo;
}


Entity ToriSystem::AddTorus(const Position& pos, const TorusParameters& params)
{
    Entity torus = coordinator->CreateEntity();

    coordinator->AddComponent<Position>(torus, pos);

    Scale scale;
    coordinator->AddComponent<Scale>(torus, scale);

    Rotation rot;
    coordinator->AddComponent<Rotation>(torus, rot);

    coordinator->AddComponent<TorusParameters>(torus, params);
    coordinator->AddComponent<Name>(torus, nameGenerator.GenerateName("Torus"));

    auto vertices = params.GenerateVertices();
    auto indices = params.GenerateEdges();

    Mesh mesh;
    mesh.Update(vertices, indices);
    coordinator->AddComponent<Mesh>(torus, mesh);

    return torus;
}


void ToriSystem::SetTorusParameter(Entity entity, const TorusParameters& params)
{
    coordinator->SetComponent<TorusParameters>(entity, params);

    auto vertices = params.GenerateVertices();
    auto indices = params.GenerateEdges();

    Mesh mesh = coordinator->GetComponent<Mesh>(entity);
    mesh.Update(vertices, indices);
    coordinator->SetComponent<Mesh>(entity, mesh);
}


void ToriSystem::Render()
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shadersRepo->GetStdShader();

    glm::mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(glm::vec4(1.0f));

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        auto const& position = coordinator->GetComponent<Position>(entity);
        auto const& scale = coordinator->GetComponent<Scale>(entity);
        auto const& rotation = coordinator->GetComponent<Rotation>(entity);
        
        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

        glm::mat4x4 modelMtx = position.TranslationMatrix() * rotation.GetRotationMatrix() * scale.ScaleMatrix();
        shader.SetMVP(cameraMtx * modelMtx);
        
        mesh.Use();
        glDrawElements(GL_LINE_LOOP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
        
        if (selection)
            shader.SetColor(glm::vec4(1.0f));
    }
}
