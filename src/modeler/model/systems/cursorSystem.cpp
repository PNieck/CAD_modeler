#include <CAD_modeler/model/systems/cursorSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/name.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>


void CursorSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<CursorSystem>();

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glPointSize(10.0f);
}


void CursorSystem::Init(ShaderRepository* shadersRepo)
{
    this->shaderRepo = shadersRepo;

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
    auto const& shader = shaderRepo->GetStdShader();

    glm::mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    auto const& mesh = coordinator->GetComponent<Mesh>(cursor);
    auto const& position = coordinator->GetComponent<Position>(cursor);
    bool selected = selectionSystem->IsSelected(cursor);

    shader.Use();

    if (selected)
        shader.SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    else
        shader.SetColor(glm::vec4(0.5f, 0.0f, 1.0f, 1.0f));

    glm::mat4x4 modelMtx = position.TranslationMatrix();
    shader.SetMVP(cameraMtx * modelMtx);

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
