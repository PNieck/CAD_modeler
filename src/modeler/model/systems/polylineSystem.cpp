#include "CAD_modeler/model/systems/polylineSystem.hpp"

#include "CAD_modeler/model/components/mesh.hpp"

#include "CAD_modeler/model/systems/shaders/shaderRepository.hpp"

#include "ecs/coordinator.hpp"

#include <numeric>


void PolylineSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<PolylineSystem>();
}


Entity PolylineSystem::AddPolyline(const std::vector<Position> &pos)
{
    const Entity polyline = coordinator->CreateEntity();

    Mesh mesh;

    mesh.Update(
        GenerateBezierPolygonVertices(pos),
        GenerateBezierPolygonIndices(pos)
    );

    coordinator->AddComponent(polyline, mesh);

    entities.insert(polyline);

    return polyline;
}


void PolylineSystem::Render(const alg::Mat4x4 &cameraMtx) const
{
    if (entities.empty())
        return;

    auto const& shader = ShaderRepository::GetInstance().GetStdShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (const Entity entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
    }
}


std::vector<float> PolylineSystem::GenerateBezierPolygonVertices(const std::vector<Position> &cps) const
{
    std::vector<float> result;
    result.reserve(cps.size() * alg::Vec3::dim);

    for (const Position& pos : cps) {
        result.push_back(pos.GetX());
        result.push_back(pos.GetY());
        result.push_back(pos.GetZ());
    }

    return result;
}


std::vector<uint32_t> PolylineSystem::GenerateBezierPolygonIndices(const std::vector<Position> &cps) const
{
    std::vector<uint32_t> result(cps.size());

    std::iota(result.begin(), result.end(), 0);

    return result;
}



