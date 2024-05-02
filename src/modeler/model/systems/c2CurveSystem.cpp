#include <CAD_modeler/model/systems/c2CurveSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/name.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/curveControlPoints.hpp>
#include <CAD_modeler/model/components/c2CurveParameters.hpp>
#include <CAD_modeler/model/components/bSplinePolygonMesh.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/toUpdateSystem.hpp>
#include <CAD_modeler/model/systems/curveControlPointsSystem.hpp>

#include <CAD_modeler/utilities/setIntersection.hpp>

#include <algorithm>


void C2CurveSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<C2CurveSystem>();

    coordinator.RegisterRequiredComponent<C2CurveSystem, C2CurveParameters>();
    coordinator.RegisterRequiredComponent<C2CurveSystem, CurveControlPoints>();
    coordinator.RegisterRequiredComponent<C2CurveSystem, Mesh>();
}


Entity C2CurveSystem::CreateC2Curve(const std::vector<Entity>& entities)
{
    Entity curve = coordinator->GetSystem<CurveControlPointsSystem>()->CreateControlPoints(entities);
    auto const& controlPoints = coordinator->GetComponent<CurveControlPoints>(curve);

    C2CurveParameters params;

    Mesh mesh;
    mesh.Update(
        GenerateCurveMeshVertices(controlPoints),
        GenerateCurveMeshIndices(controlPoints)
    );

    coordinator->AddComponent<C2CurveParameters>(curve, params);
    coordinator->AddComponent<Mesh>(curve, mesh);
    coordinator->AddComponent<Name>(curve, nameGenerator.GenerateName("CurveC2_"));

    return curve;
}


void C2CurveSystem::ShowBSplinePolygon(Entity entity)
{
    coordinator->EditComponent<C2CurveParameters>(entity,
        [](C2CurveParameters& params) {
            params.drawBSplinePolygon = true;
        }
    );

    auto const& controlPoints = coordinator->GetComponent<CurveControlPoints>(entity);
    BSplinePolygonMesh polygonMesh;

    polygonMesh.Update(
        GenerateBSplinePolygonVertices(controlPoints),
        GenerateBSplinePolygonIndices(controlPoints)
    );

    coordinator->AddComponent<BSplinePolygonMesh>(entity, polygonMesh);
}


void C2CurveSystem::HideBSplinePolygon(Entity entity)
{
    coordinator->EditComponent<C2CurveParameters>(entity,
        [](C2CurveParameters& params) {
            params.drawBSplinePolygon = false;
        }
    );

    coordinator->DeleteComponent<BSplinePolygonMesh>(entity);
}


void C2CurveSystem::Render() const
{
     if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();

    auto const& shader = shaderRepo->GetBezierShader();
    std::stack<Entity> bSplinePolygonsToDraw;

    UpdateEntities();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (auto const entity: entities) {
        auto const& params = coordinator->GetComponent<C2CurveParameters>(entity);
        if (params.drawBSplinePolygon)
            bSplinePolygonsToDraw.push(entity);

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

    if (!bSplinePolygonsToDraw.empty())
        RenderBSplinePolygons(bSplinePolygonsToDraw);
}


void C2CurveSystem::UpdateEntities() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();

    auto toUpdate = intersect(toUpdateSystem->GetEntities(), entities);

    for (auto entity: toUpdate) {
        UpdateMesh(entity);
        coordinator->GetSystem<ToUpdateSystem>()->Unmark(entity);

        if (coordinator->GetComponent<C2CurveParameters>(entity).drawBSplinePolygon) {
            UpdateBSplinePolygon(entity);
        }
    }
}


void C2CurveSystem::UpdateMesh(Entity curve) const
{
    coordinator->EditComponent<Mesh>(curve,
        [curve, this](Mesh& mesh) {
            auto const& params = coordinator->GetComponent<CurveControlPoints>(curve);

            mesh.Update(
                GenerateCurveMeshVertices(params),
                GenerateCurveMeshIndices(params)
            );
        }
    );
}


void C2CurveSystem::UpdateBSplinePolygon(Entity curve) const
{
    coordinator->EditComponent<BSplinePolygonMesh>(curve,
        [curve, this](BSplinePolygonMesh& mesh) {
            auto const& params = coordinator->GetComponent<CurveControlPoints>(curve);

            mesh.Update(
                GenerateBSplinePolygonVertices(params),
                GenerateBSplinePolygonIndices(params)
            );
        }
    );
}


void C2CurveSystem::RenderBSplinePolygons(std::stack<Entity>& entities) const
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

        auto const& mesh = coordinator->GetComponent<BSplinePolygonMesh>(entity);
        mesh.Use();

	    glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


std::vector<float> C2CurveSystem::GenerateCurveMeshVertices(const CurveControlPoints& params) const
{
    auto const& controlPoints = params.ControlPoints();

    std::vector<float> result;

    if (controlPoints.size() < 4)
        return result;

    result.reserve((controlPoints.size() - 3)*3);

    for (int i=0; i < controlPoints.size() - 3 ; ++i) {
        auto const& pos1 = coordinator->GetComponent<Position>(controlPoints[i]);
        auto const& pos2 = coordinator->GetComponent<Position>(controlPoints[i+1]);
        auto const& pos3 = coordinator->GetComponent<Position>(controlPoints[i+2]);
        auto const& pos4 = coordinator->GetComponent<Position>(controlPoints[i+3]);

        alg::Vec3 g0 = (2.f/3.f) * pos1.vec + (1.f/3.f) * pos2.vec;
        alg::Vec3 f1 = (1.f/3.f) * pos2.vec + (2.f/3.f) * pos3.vec;
        alg::Vec3 e0 = 0.5f * g0 + 0.5f * f1;

        alg::Vec3 g1 = (2.f/3.f) * pos2.vec + (1.f/3.f) * pos3.vec;
        alg::Vec3 f2 = (1.f/3.f) * pos3.vec + (2.f/3.f) * pos4.vec;
        alg::Vec3 e1 = 0.5f * g1 + 0.5f * f2;

        result.push_back(e0.X());
        result.push_back(e0.Y());
        result.push_back(e0.Z());

        result.push_back(f1.X());
        result.push_back(f1.Y());
        result.push_back(f1.Z());

        result.push_back(g1.X());
        result.push_back(g1.Y());
        result.push_back(g1.Z());

        result.push_back(e1.X());
        result.push_back(e1.Y());
        result.push_back(e1.Z());
    }

    return result;
}


std::vector<uint32_t> C2CurveSystem::GenerateCurveMeshIndices(const CurveControlPoints& params) const
{
    auto const& controlPoints = params.ControlPoints();
    size_t segments = std::max<size_t>(controlPoints.size() - 3, 0);
    std::vector<uint32_t> result(4 * segments);

    for (int i = 0; i < result.size(); ++i) {
        result[i] = i;
    }

    return result;
}


std::vector<float> C2CurveSystem::GenerateBSplinePolygonVertices(const CurveControlPoints & params) const
{
    auto const& controlPoints = params.ControlPoints();

    std::vector<float> result;
    result.reserve(3 * controlPoints.size());

    for (Entity entity: controlPoints) {
        auto const& pos = coordinator->GetComponent<Position>(entity);

        result.push_back(pos.GetX());
        result.push_back(pos.GetY());
        result.push_back(pos.GetZ());
    }

    return result;
}


std::vector<uint32_t> C2CurveSystem::GenerateBSplinePolygonIndices(const CurveControlPoints & params) const
{
    auto const& controlPoints = params.ControlPoints();

    std::vector<uint32_t> result(3 * controlPoints.size());

    for (int i = 0; i < result.size(); ++i) {
        result[i] = i;
    }

    return result;
}
