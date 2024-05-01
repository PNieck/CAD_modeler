#include <CAD_modeler/model/systems/c2CurveSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/name.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/curveControlPoints.hpp>
#include <CAD_modeler/model/components/c2CurveParameters.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>


void C2CurveSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<C2CurveSystem>();

    coordinator.RegisterRequiredComponent<C2CurveSystem, C2CurveParameters>();
    coordinator.RegisterRequiredComponent<C2CurveSystem, CurveControlPoints>();
    coordinator.RegisterRequiredComponent<C2CurveSystem, Mesh>();
}


Entity C2CurveSystem::CreateC2Curve(const std::vector<Entity>& entities)
{
    Entity bezierCurve = coordinator->CreateEntity();

    C2CurveParameters params;
    CurveControlPoints controlPoints(entities);

    Mesh mesh;
    mesh.Update(
        GenerateBezierPolygonVertices(controlPoints),
        GenerateBezierPolygonIndices(controlPoints)
    );

    // auto handler = std::make_shared<RecalculateMeshHandler>(bezierCurve, *this);

    // for (Entity entity: entities) {
    //     auto handlerId = coordinator->Subscribe<Position>(entity, std::static_pointer_cast<EventHandler<Position>>(handler));
    //     params.handlers.insert({ entity, handlerId });
    // }

    // params.parameterDeletionHandler = coordinator->Subscribe<C0CurveParameters>(bezierCurve, std::static_pointer_cast<EventHandler<C0CurveParameters>>(parameterDeletionHandler));

    coordinator->AddComponent<C2CurveParameters>(bezierCurve, params);
    coordinator->AddComponent<CurveControlPoints>(bezierCurve, controlPoints);
    coordinator->AddComponent<Mesh>(bezierCurve, mesh);
    coordinator->AddComponent<Name>(bezierCurve, nameGenerator.GenerateName("CurveC2_"));

    return bezierCurve;


}


void C2CurveSystem::AddControlPoint(Entity bezierCurve, Entity entity)
{
}


void C2CurveSystem::DeleteControlPoint(Entity bezierCurve, Entity entity)
{
}


void C2CurveSystem::Render() const
{
     if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetBezierShader();
    std::stack<Entity> polygonsToDraw;

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (auto const entity: entities) {
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

    // if (!polygonsToDraw.empty())
    //     RenderCurvesPolygons(polygonsToDraw);
}


std::vector<float> C2CurveSystem::GenerateBezierPolygonVertices(const CurveControlPoints& params) const
{
    std::vector<float> result;

    auto const& controlPoints = params.ControlPoints();

    auto const& pos1 = coordinator->GetComponent<Position>(controlPoints[0]);
    auto const& pos2 = coordinator->GetComponent<Position>(controlPoints[1]);
    auto const& pos3 = coordinator->GetComponent<Position>(controlPoints[2]);
    auto const& pos4 = coordinator->GetComponent<Position>(controlPoints[3]);

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

    return result;
}


std::vector<uint32_t> C2CurveSystem::GenerateBezierPolygonIndices(const CurveControlPoints& params) const
{
    std::vector<uint32_t> result = {
        0,1 ,2, 3
    };

    return result;
}
