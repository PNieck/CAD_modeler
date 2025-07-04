#include <CAD_modeler/model/systems/pointsSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>

#include <CAD_modeler/model/components/name.hpp>
#include <CAD_modeler/model/components/position.hpp>


void PointsSystem::RegisterSystem(Coordinator& coordinator)
{
    coordinator.RegisterSystem<PointsSystem>();
}


PointsSystem::PointsSystem()
{
    const std::vector vertices = {
        0.0f, 0.0f, 0.0f
    };

    const std::vector<uint32_t> indices = { 0 };

    pointsMesh.Update(vertices, indices);
}


Entity PointsSystem::CreatePoint(const Position& pos)
{
    const Entity newPoint = coordinator->CreateEntity();

    coordinator->AddComponent<Position>(newPoint, pos);
    entities.insert(newPoint);

    return newPoint;
}


void PointsSystem::AddPoint(const Entity e)
{
    if (!coordinator->HasComponent<Position>(e))
        throw std::runtime_error("Entity has no position");

    entities.insert(e);
}


void PointsSystem::Render(const alg::Mat4x4& cameraMtx) const
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetStdShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));

    for (auto const entity : entities) {
        auto const& position = coordinator->GetComponent<Position>(entity);

        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        shader.SetMVP(cameraMtx * position.TranslationMatrix());
        
        pointsMesh.Use();
        glDrawElements(GL_POINTS, pointsMesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}
