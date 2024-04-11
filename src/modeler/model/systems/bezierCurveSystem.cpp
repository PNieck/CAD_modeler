#include <CAD_modeler/model/systems/bezierCurveSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/bezierCurveParameters.hpp>
#include <CAD_modeler/model/components/bezierMesh.hpp>
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
    coordinator.RegisterRequiredComponent<BezierCurveSystem, BezierMesh>();
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


    BezierMesh bezierMesh;
    auto meshVals = CalculateAdaptableBezierMesh(params);

    bezierMesh.Update(
        std::get<0>(meshVals),
        std::get<1>(meshVals)
    );
    
    RecalculateMeshEvent callback(bezierCurve, *this);

    for (Entity entity: entities) {
        auto handlerId = coordinator->SubscribeToComponentChange<Position>(entity, callback);
        params.handlers.insert({ entity, handlerId });
    }

    Entity camera = coordinator->GetSystem<CameraSystem>()->GetCameraEntity();
    params.cameraMovedEvent = coordinator->SubscribeToComponentChange<Position>(camera, callback);

    coordinator->AddComponent<BezierCurveParameter>(bezierCurve, params);
    coordinator->AddComponent<Mesh>(bezierCurve, mesh);
    coordinator->AddComponent<BezierMesh>(bezierCurve, bezierMesh);

    return bezierCurve;
}


void BezierCurveSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    glm::mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(glm::vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (auto const entity: entities) {
        auto const& params = coordinator->GetComponent<BezierCurveParameter>(entity);

        if (params.drawPolygon) {
            auto const& mesh = coordinator->GetComponent<Mesh>(entity);

            bool selection = selectionSystem->IsSelected(entity);

            if (selection)
                shader.SetColor(glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

            mesh.Use();
            glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

            if (selection)
                shader.SetColor(glm::vec4(1.0f));
        }

        auto const& bezierMesh = coordinator->GetComponent<BezierMesh>(entity);

        bezierMesh.Use();
        glDrawElements(GL_LINE_STRIP, bezierMesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
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


std::vector<float> BezierCurveSystem::CalculateBezierMesh(const BezierCurveParameter & params) const
{
    static constexpr int segments = 2000;

    std::vector<float> result(segments * 3);
    float t = 0.0f;

    auto const& controlPoints = params.ControlPoints();

    auto const& pos0 = coordinator->GetComponent<Position>(controlPoints[0]);
    auto const& pos1 = coordinator->GetComponent<Position>(controlPoints[1]);
    auto const& pos2 = coordinator->GetComponent<Position>(controlPoints[2]);
    auto const& pos3 = coordinator->GetComponent<Position>(controlPoints[3]);

    for (int i=0; i < segments; i++) {
        glm::vec3 pos = std::powf(1-t, 3) * pos0.vec +
            3 * std::powf(1-t, 2) * t * pos1.vec +
            3 * (1-t) * std::powf(t, 2) * pos2.vec +
            std::powf(t, 3) * pos3.vec;

        result[3*i] = pos.x;
        result[3*i + 1] = pos.y;
        result[3*i + 2] = pos.z;

        t += 1.0f/static_cast<float>(segments - 1);
    }

    return result;
}


std::vector<uint32_t> BezierCurveSystem::CalculateBezierIndices(const BezierCurveParameter & params) const
{
    static constexpr int segments = 2000;

    std::vector<uint32_t> result(segments);

    for (int i=0; i < segments; i++) {
        result[i] = i;
    }

    return result;
}


std::tuple<std::vector<float>,std::vector<uint32_t>> BezierCurveSystem::CalculateAdaptableBezierMesh(const BezierCurveParameter & params) const
{
    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();

    auto const& controlPoints = params.ControlPoints();
    auto const cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    auto const& pos0 = coordinator->GetComponent<Position>(controlPoints[0]);
    auto const& pos1 = coordinator->GetComponent<Position>(controlPoints[1]);
    auto const& pos2 = coordinator->GetComponent<Position>(controlPoints[2]);
    auto const& pos3 = coordinator->GetComponent<Position>(controlPoints[3]);

    auto const proj0 = ProjectPoint(pos0.vec, cameraMtx);
    auto const proj1 = ProjectPoint(pos1.vec, cameraMtx);
    auto const proj2 = ProjectPoint(pos2.vec, cameraMtx);
    auto const proj3 = ProjectPoint(pos3.vec, cameraMtx);

    float maxX = std::max(std::max(proj0.x, proj1.x), std::max(proj2.x, proj3.x));
    float maxY = std::max(std::max(proj0.y, proj1.y), std::max(proj2.y, proj3.y));

    float minX = std::min(std::min(proj0.x, proj1.x), std::min(proj2.x, proj3.x));
    float minY = std::min(std::min(proj0.y, proj1.y), std::min(proj2.y, proj3.y));

    int segments;
    if (maxX - minX > maxY - minY) {
        segments = (int)(maxX - minX) * cameraSystem->GetViewportWidth();
    } else {
        segments = (int)(maxY - minY) * cameraSystem->GetViewportHeight();
    }

    segments /= 5;

    std::vector<float> vertices(segments * 3);
    std::vector<uint32_t> indices(segments);

    float t = 0.0f;
    float delta = 1.0f/static_cast<float>(segments);
    for (int i=0; i < segments; i++) {
        glm::vec3 pos = std::powf(1-t, 3) * pos0.vec +
            3 * std::powf(1-t, 2) * t * pos1.vec +
            3 * (1-t) * std::powf(t, 2) * pos2.vec +
            std::powf(t, 3) * pos3.vec;

        vertices[3*i] = pos.x;
        vertices[3*i + 1] = pos.y;
        vertices[3*i + 2] = pos.z;

        t += delta;
    }

    //assert(t >= 1.0f);

    for (int i=0; i < segments; i++) {
        indices[i] = i;
    }

    return make_tuple(vertices, indices);
}


glm::vec3 BezierCurveSystem::ProjectPoint(const glm::vec3 & point, const glm::mat4x4 mat)
{
    glm::vec4 tmp(point, 1.0f);
    tmp =  tmp * mat;

    return glm::vec3(tmp.x / tmp.w, tmp.y / tmp.w, tmp.z / tmp.w);
}


void BezierCurveSystem::RecalculateMeshEvent::operator()(Entity entity, const Position & pos) const
{
    auto mesh = bezierSystem.coordinator->GetComponent<Mesh>(bezierCurve);
    auto bezierMesh = bezierSystem.coordinator->GetComponent<BezierMesh>(bezierCurve);
    auto const& params = bezierSystem.coordinator->GetComponent<BezierCurveParameter>(bezierCurve);

    mesh.Update(
        bezierSystem.GenerateBezierPolygonVertices(params),
        bezierSystem.GenerateBezierPolygonIndices(params)
    );

    auto meshVals = bezierSystem.CalculateAdaptableBezierMesh(params);
    bezierMesh.Update(
        std::get<0>(meshVals),
        std::get<1>(meshVals)
    );

    bezierSystem.coordinator->SetComponent<Mesh>(bezierCurve, bezierMesh);
}
