#include <CAD_modeler/model/systems/cursorSystem.hpp>

#include <CAD_modeler/model/ecsCoordinator.hpp>

#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/name.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>


void CursorSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<CursorSystem>();

    coordinator.RegisterComponent<Position>();
    coordinator.RegisterComponent<Mesh>();
    coordinator.RegisterComponent<Name>();

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glPointSize(10.0f);
}


CursorSystem::CursorSystem():
    shader("../../shaders/vertexShader.vert", "../../shaders/fragmentShader.frag")
{
}


void CursorSystem::Init()
{
    cursor = coordinator->CreateEntity();

    Position pos(0.0f);
    coordinator->AddComponent<Position>(cursor, pos);

    Mesh mesh;
    std::vector<float> vertices {
        0.0f, 0.0f, 0.0f
    };
    std::vector<uint32_t> indices { 0 };
    
    mesh.Update(vertices, indices);
    coordinator->AddComponent<Mesh>(cursor, mesh);

    coordinator->AddComponent<Name>(cursor, "Cursor");
}


void CursorSystem::Render()
{
    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();

    glm::mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    auto const& mesh = coordinator->GetComponent<Mesh>(cursor);
    auto const& position = coordinator->GetComponent<Position>(cursor);
    bool selected = selectionSystem->IsSelected(cursor);

    shader.use();

    if (selected)
        shader.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    else
        shader.setVec4("color", glm::vec4(0.5f, 0.0f, 1.0f, 1.0f));

    glm::mat4x4 modelMtx = position.TranslationMatrix();
    shader.setMatrix4("MVP", cameraMtx * modelMtx);

    mesh.Use();
    glDrawElements(GL_POINTS, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
}


void CursorSystem::SetPosition(const glm::vec3 & pos)
{
    auto& position = coordinator->GetComponent<Position>(cursor);
    position.vec = pos;
}


Position CursorSystem::GetPosition() const
{
    return coordinator->GetComponent<Position>(cursor);
}
