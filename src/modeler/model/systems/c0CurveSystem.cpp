#include <CAD_modeler/model/systems/c0CurveSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/c0CurveParameters.hpp>
#include <CAD_modeler/model/components/curveControlPoints.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/toUpdate.hpp>
#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/name.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>
#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <CAD_modeler/utilities/setIntersection.hpp>

#include <cmath>
#include <algorithm>


static constexpr int COORD_IN_VERTEX = 3;


void C0CurveSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<C0CurveSystem>();

    coordinator.RegisterRequiredComponent<C0CurveSystem, C0CurveParameters>();
    coordinator.RegisterRequiredComponent<C0CurveSystem, CurveControlPoints>();
    coordinator.RegisterRequiredComponent<C0CurveSystem, Mesh>();
}


Entity C0CurveSystem::CreateC0Curve(const std::vector<Entity>& entities)
{
    Entity curve = coordinator->GetSystem<CurveControlPointsSystem>()->CreateControlPoints(entities);

    C0CurveParameters params;

    auto const& controlPoints = coordinator->GetComponent<CurveControlPoints>(curve);

    Mesh mesh;
    mesh.Update(
        GenerateBezierPolygonVertices(controlPoints),
        GenerateBezierPolygonIndices(controlPoints)
    );

    coordinator->AddComponent<C0CurveParameters>(curve, params);
    coordinator->AddComponent<Mesh>(curve, mesh);
    coordinator->AddComponent<Name>(curve, nameGenerator.GenerateName("CurveC0_"));

    return curve;
}


void C0CurveSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();

    auto const& shader = shaderRepo->GetBezierShader();
    std::stack<Entity> polygonsToDraw;

    UpdateEntities();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (auto const entity: entities) {
        auto const& controlPoints = coordinator->GetComponent<CurveControlPoints>(entity);
        auto const& params = coordinator->GetComponent<C0CurveParameters>(entity);

        if (params.drawPolygon)
            polygonsToDraw.push(entity);

        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        glPatchParameteri(GL_PATCH_VERTICES, 4);
	    glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }

    if (!polygonsToDraw.empty())
        RenderCurvesPolygons(polygonsToDraw);
}


void C0CurveSystem::RenderCurvesPolygons(std::stack<Entity>& entities) const
{
    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    while (!entities.empty()) {
        Entity entity = entities.top();
        entities.pop();

        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

	    glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void C0CurveSystem::UpdateEntities() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();

    auto toUpdate = intersect(toUpdateSystem->GetEntities(), entities);

    for (auto entity: toUpdate) {
        UpdateMesh(entity);
        coordinator->GetSystem<ToUpdateSystem>()->Unmark(entity);
    }
}


void C0CurveSystem::UpdateMesh(Entity bezierCurve) const
{
    coordinator->EditComponent<Mesh>(bezierCurve,
        [bezierCurve, this](Mesh& mesh) {
            auto const& params = coordinator->GetComponent<CurveControlPoints>(bezierCurve);

            mesh.Update(
                GenerateBezierPolygonVertices(params),
                GenerateBezierPolygonIndices(params)
            );
        }
    );
}


std::vector<float> C0CurveSystem::GenerateBezierPolygonVertices(const CurveControlPoints& params) const
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


int ceiling(int x, int y) {
    return (x + y - 1) / y;
}


std::vector<uint32_t> C0CurveSystem::GenerateBezierPolygonIndices(const CurveControlPoints& params) const
{
    auto const& controlPoints = params.ControlPoints();

    int fullSegmentsCnt = (controlPoints.size() - 1) / (CONTROL_POINTS_PER_SEGMENT - 1);
    int allSegmentsCnt = ceiling(controlPoints.size() - 1, CONTROL_POINTS_PER_SEGMENT - 1);
    int resultSize = allSegmentsCnt * CONTROL_POINTS_PER_SEGMENT;

    std::vector<uint32_t> result;
    result.reserve(resultSize);

    // Add all full segments
    int firstControlPoint = 0;
    for (; firstControlPoint < fullSegmentsCnt * CONTROL_POINTS_PER_SEGMENT - 3; firstControlPoint += 3) {
        result.push_back(firstControlPoint);
        result.push_back(firstControlPoint + 1);
        result.push_back(firstControlPoint + 2);
        result.push_back(firstControlPoint + 3);
    }

    // Add the rest of control points
    int i = firstControlPoint;
    while (i < controlPoints.size()) {
        result.push_back(i++);
    }

    // Repet the last control point
    while (result.size() < resultSize) {
        result.push_back(controlPoints.size() - 1);
    }
    

    return result;
}
