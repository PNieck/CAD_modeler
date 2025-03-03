#include <CAD_modeler/model/systems/cursorSystem.hpp>

#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/mesh.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>


void CursorSystem::RegisterSystem(Coordinator& coordinator)
{
    coordinator.RegisterSystem<CursorSystem>();
}


void CursorSystem::Init()
{
    cursor = coordinator->CreateEntity();

    const Position pos(0.0f);
    coordinator->AddComponent<Position>(cursor, pos);

    Mesh mesh;
    const std::vector<float> vertices {
        0.0f, 0.0f, 0.0f
    };
    const std::vector<uint32_t> indices { 0 };
    
    mesh.Update(vertices, indices);
    
    coordinator->AddComponent<Mesh>(cursor, mesh);
    
}


void CursorSystem::Render(const alg::Mat4x4& cameraMtx)
{

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetStdShader();

    auto const& mesh = coordinator->GetComponent<Mesh>(cursor);
    auto const& position = coordinator->GetComponent<Position>(cursor);
    const bool selected = selectionSystem->IsSelected(cursor);

    shader.Use();

    if (selected)
        shader.SetColor(alg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    else
        shader.SetColor(alg::Vec4(0.5f, 0.0f, 1.0f, 1.0f));

    const alg::Mat4x4 modelMtx = position.TranslationMatrix();
    shader.SetMVP(cameraMtx * modelMtx);

    mesh.Use();
    glDrawElements(GL_POINTS, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
}
