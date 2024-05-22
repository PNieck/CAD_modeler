#include <CAD_modeler/model/systems/c0SurfaceSystem.hpp>

#include <ecs/coordinator.hpp>

#include <algebra/vec3.hpp>

#include "CAD_modeler/model/systems/pointsSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsSystem.hpp"
#include "CAD_modeler/model/systems/cameraSystem.hpp"
#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"

#include "CAD_modeler/model/components/c0SurfacePatches.hpp"
#include "CAD_modeler/model/components/mesh.hpp"
#include "CAD_modeler/model/components/name.hpp"

#include <CAD_modeler/utilities/setIntersection.hpp>

#include <vector>


const alg::Vec3 C0SurfaceSystem::offsetX = alg::Vec3(1.f/3.f, 0.f, 0.f);
const alg::Vec3 C0SurfaceSystem::offsetZ = alg::Vec3(0.f, 0.f, -1.f/3.f);


void C0SurfaceSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C0SurfaceSystem>();

    coordinator.RegisterRequiredComponent<C0SurfaceSystem, C0SurfacePatches>();
    coordinator.RegisterRequiredComponent<C0SurfaceSystem, ControlPoints>();
    coordinator.RegisterRequiredComponent<C0SurfaceSystem, Mesh>();
}


Entity C0SurfaceSystem::CreateSurface(const Position& pos)
{
    static constexpr int controlPointsInOneDir = 4;
    static constexpr int controlPointsCnt = controlPointsInOneDir*controlPointsInOneDir;

    std::vector<Entity> controlPoints;
    C0SurfacePatches patches(1, 1);
    controlPoints.reserve(controlPointsCnt);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();

    alg::Vec3 actPos = pos.vec;

    for (int i=0; i < controlPointsInOneDir; ++i) {
        for (int j=0; j < controlPointsInOneDir; ++j) {
            Entity cp = pointsSystem->CreatePoint(actPos);
            controlPoints.push_back(cp);
            patches.SetPoint(cp, 0, 0, i, j);

            actPos += offsetX;
        }

        actPos += offsetZ;
        actPos -= static_cast<float>(controlPointsInOneDir) * offsetX;
    }

    auto const controlPointsSys = coordinator->GetSystem<ControlPointsSystem>();
    Entity surface = controlPointsSys->CreateControlPoints(controlPoints);

    Mesh mesh;

    mesh.Update(
        GenerateVertices(patches),
        GenerateIndices(patches)
    );

    coordinator->AddComponent<Name>(surface, nameGenerator.GenerateName("SurfaceC0_"));
    coordinator->AddComponent<Mesh>(surface, mesh);
    coordinator->AddComponent<C0SurfacePatches>(surface, patches);

    return surface;
}


void C0SurfaceSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();

    auto const& shader = shaderRepo->GetBezierSurfaceShader();
    std::stack<Entity> polygonsToDraw;

    UpdateEntities();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (auto const entity: entities) {
        auto const& controlPoints = coordinator->GetComponent<ControlPoints>(entity);

        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        glPatchParameteri(GL_PATCH_VERTICES, 16);
        float v[] = {5.f, 5.f, 5.f, 5.f};
        glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, v);
        float v2[] = {5.f, 5.f};
        glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, v2);

	    glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void C0SurfaceSystem::AddPatchesInUDir(Entity surface) const
{
    coordinator->EditComponent<ControlPoints>(surface,
        [surface, this](ControlPoints& points) {
            auto const& patches = coordinator->GetComponent<C0SurfacePatches>(surface);


        }
    );
}


void C0SurfaceSystem::AddPatchesInVDir(Entity surface) const
{
}


void C0SurfaceSystem::DeletePatchesInUDir(Entity surface) const
{
}


void C0SurfaceSystem::DeletePatchesInVDir(Entity surface) const
{
}


void C0SurfaceSystem::UpdateEntities() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();

    auto toUpdate = intersect(toUpdateSystem->GetEntities(), entities);

    for (auto entity: toUpdate) {
        UpdateMesh(entity);
        coordinator->GetSystem<ToUpdateSystem>()->Unmark(entity);
    }
}


void C0SurfaceSystem::UpdateMesh(Entity surface) const
{
    coordinator->EditComponent<Mesh>(surface,
        [surface, this](Mesh& mesh) {
            auto const& patches = coordinator->GetComponent<C0SurfacePatches>(surface);

            mesh.Update(
                GenerateVertices(patches),
                GenerateIndices(patches)
            );
        }
    );
}


std::vector<float> C0SurfaceSystem::GenerateVertices(const C0SurfacePatches& patches) const
{
    std::vector<float> result;
    result.reserve(patches.PointsCnt() * 3);

    for (int row=0; row < C0SurfacePatches::RowsInPatch; row++) {
        for (int col=0; col < C0SurfacePatches::ColsInPatch; col++) {
            Entity point = patches.GetPoint(0, 0, row, col);

            auto const& pos = coordinator->GetComponent<Position>(point);

            result.push_back(pos.GetX());
            result.push_back(pos.GetY());
            result.push_back(pos.GetZ());
        }
    }

    return result;
}


std::vector<uint32_t> C0SurfaceSystem::GenerateIndices(const C0SurfacePatches& patches) const
{
    std::vector<uint32_t> result(patches.PointsCnt());

    for (int i=0; i < patches.PointsCnt(); i++) {
        result[i] = i;
    }

    return result;
}
