#include <CAD_modeler/model/systems/bezierCurveSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/bezierCurveParameters.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/mesh.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <cmath>
#include <algorithm>


static constexpr int COORD_IN_VERTEX = 3;


void BezierCurveSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<BezierCurveSystem>();

    coordinator.RegisterRequiredComponent<BezierCurveSystem, BezierCurveParameter>();
    coordinator.RegisterRequiredComponent<BezierCurveSystem, Mesh>();
}


Entity BezierCurveSystem::CreateBezierCurve(const std::vector<Entity>& entities)
{
    Entity bezierCurve = coordinator->CreateEntity();

    BezierCurveParameter params(entities);
    params.drawPolygon = true;

    Mesh mesh;
    mesh.Update(
        GenerateBezierPolygonVertices(params),
        GenerateBezierPolygonIndices(params)
    );
    
    RecalculateMeshEvent callback(bezierCurve, *this);

    for (Entity entity: entities) {
        auto handlerId = coordinator->SubscribeToComponentChange<Position>(entity, callback);
        params.handlers.insert({ entity, handlerId });
    }

    coordinator->AddComponent<BezierCurveParameter>(bezierCurve, params);
    coordinator->AddComponent<Mesh>(bezierCurve, mesh);

    return bezierCurve;
}


void BezierCurveSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetBezierShader();

    glm::mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(glm::vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (auto const entity: entities) {
        auto const& params = coordinator->GetComponent<BezierCurveParameter>(entity);

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        glPatchParameteri( GL_PATCH_VERTICES, 4 );
	    glDrawArrays( GL_PATCHES, 0, mesh.GetElementsCnt());
    }
}


std::vector<float> BezierCurveSystem::GenerateBezierPolygonVertices(const BezierCurveParameter& params) const
{
    auto const& controlPoints = params.ControlPoints();

    std::vector<float> result(controlPoints.size() * COORD_IN_VERTEX);

    int i = 0;
    for (Entity entity: controlPoints) {
        auto const& pos = coordinator->GetComponent<Position>(entity);

        result[i * COORD_IN_VERTEX] = pos.GetX();
        result[i * COORD_IN_VERTEX + 1] = pos.GetY();
        result[i * COORD_IN_VERTEX + 2] = pos.GetZ();

        i++;
    }

    return result;
}


std::vector<uint32_t> BezierCurveSystem::GenerateBezierPolygonIndices(const BezierCurveParameter& params) const
{
    auto const& controlPoints = params.ControlPoints();

    std::vector<uint32_t> result(controlPoints.size());

    for (int i=0; i < controlPoints.size(); i++) {
        result[i] = i;
    }

    return result;
}


void BezierCurveSystem::RecalculateMeshEvent::operator()(Entity entity, const Position & pos) const
{
    auto mesh = bezierSystem.coordinator->GetComponent<Mesh>(bezierCurve);
    auto const& params = bezierSystem.coordinator->GetComponent<BezierCurveParameter>(bezierCurve);

    mesh.Update(
        bezierSystem.GenerateBezierPolygonVertices(params),
        bezierSystem.GenerateBezierPolygonIndices(params)
    );

    bezierSystem.coordinator->SetComponent<Mesh>(bezierCurve, mesh);
}
