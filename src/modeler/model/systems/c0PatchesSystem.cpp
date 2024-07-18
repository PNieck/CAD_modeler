#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>

#include "CAD_modeler/model/components/c0SurfaceDensity.hpp"
#include "CAD_modeler/model/components/curveControlPoints.hpp"
#include "CAD_modeler/model/components/mesh.hpp"
#include "CAD_modeler/model/components/patchesPolygonMesh.hpp"

#include "CAD_modeler/model/systems/cameraSystem.hpp"
#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"

#include <CAD_modeler/utilities/setIntersection.hpp>


void C0PatchesSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C0PatchesSystem>();

    coordinator.RegisterRequiredComponent<C0PatchesSystem, C0SurfacePatches>();
    coordinator.RegisterRequiredComponent<C0PatchesSystem, CurveControlPoints>();
    coordinator.RegisterRequiredComponent<C0PatchesSystem, C0SurfaceDensity>();
    coordinator.RegisterRequiredComponent<C0PatchesSystem, HasPatchesPolygon>();
    coordinator.RegisterRequiredComponent<C0PatchesSystem, Mesh>();
}


void C0PatchesSystem::ShowPolygon(Entity entity) const
{
    PatchesPolygonMesh polygonMesh;

    auto const& patches = coordinator->GetComponent<C0SurfacePatches>(entity);

    polygonMesh.Update(
        GeneratePolygonVertices(patches),
        GeneratePolygonIndices(patches)
    );

    coordinator->AddComponent(entity, polygonMesh);

    HasPatchesPolygon hasPolygon { .value = true };
    coordinator->SetComponent(entity, hasPolygon);
}


void C0PatchesSystem::HidePolygon(Entity polygon) const
{
    coordinator->DeleteComponent<PatchesPolygonMesh>(polygon);

    HasPatchesPolygon hasPolygon { .value = false };
    coordinator->SetComponent(polygon, hasPolygon);
}


void C0PatchesSystem::Render() const
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

    glPatchParameteri(GL_PATCH_VERTICES, 16);

    for (auto const entity: entities) {
        bool selection = selectionSystem->IsSelected(entity);

        if (HasPolygon(entity))
            polygonsToDraw.push(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        auto const& density = coordinator->GetComponent<C0SurfaceDensity>(entity);
        float v[] = {5.f, 64.f, 64.f, 64.f};
        v[0] = static_cast<float>(density.GetDensity());
        glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, v);

	    glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }

    RenderPolygon(polygonsToDraw);
}


void C0PatchesSystem::UpdateEntities() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();

    auto toUpdate = intersect(toUpdateSystem->GetEntities(), entities);

    for (auto entity: toUpdate) {
        UpdateMesh(entity);

        if (HasPolygon(entity))
            UpdatePolygonMesh(entity);

        toUpdateSystem->Unmark(entity);
    }
}


void C0PatchesSystem::UpdateMesh(Entity surface) const
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


void C0PatchesSystem::UpdatePolygonMesh(Entity entity) const
{
    coordinator->EditComponent<PatchesPolygonMesh>(entity,
        [entity, this] (PatchesPolygonMesh& mesh) {
            auto const& patches = coordinator->GetComponent<C0SurfacePatches>(entity);

            mesh.Update(
                GeneratePolygonVertices(patches),
                GeneratePolygonIndices(patches)
            );
        }
    );
}


void C0PatchesSystem::RenderPolygon(std::stack<Entity> &entities) const
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

        auto const& mesh = coordinator->GetComponent<PatchesPolygonMesh>(entity);
        mesh.Use();

	    glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


std::vector<float> C0PatchesSystem::GenerateVertices(const C0SurfacePatches& patches) const
{
    std::vector<float> result;
    result.reserve(patches.PointsCnt() * 3);

    for (int col=0; col < patches.PointsInCol(); col++) {
        for (int row=0; row < patches.PointsInRow(); row++) {
            Entity point = patches.GetPoint(row, col);

            auto const& pos = coordinator->GetComponent<Position>(point);

            result.push_back(pos.GetX());
            result.push_back(pos.GetY());
            result.push_back(pos.GetZ());
        }
    }

    return result;
}


std::vector<uint32_t> C0PatchesSystem::GenerateIndices(const C0SurfacePatches& patches) const
{
    std::vector<uint32_t> result;
    result.reserve(patches.Rows() * patches.Cols() * C0SurfacePatches::PointsInPatch * 2);

    for (int patchRow=0; patchRow < patches.Rows(); patchRow++) {
        for (int patchCol=0; patchCol < patches.Cols(); patchCol++) {
            for (int rowInPatch=0; rowInPatch < C0SurfacePatches::RowsInPatch; rowInPatch++) {
                for (int colInPatch=0; colInPatch < C0SurfacePatches::ColsInPatch; colInPatch++) {

                    int globCol = patchCol * (C0SurfacePatches::ColsInPatch - 1) + colInPatch;
                    int globRow = patchRow * (C0SurfacePatches::RowsInPatch - 1) + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }

            for (int colInPatch=0; colInPatch < C0SurfacePatches::ColsInPatch; colInPatch++) {
                for (int rowInPatch=0; rowInPatch < C0SurfacePatches::RowsInPatch; rowInPatch++) {

                    int globCol = patchCol * (C0SurfacePatches::ColsInPatch - 1) + colInPatch;
                    int globRow = patchRow * (C0SurfacePatches::RowsInPatch - 1) + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }
        }
    }

    return result;
}


std::vector<float> C0PatchesSystem::GeneratePolygonVertices(const C0SurfacePatches &patches) const
{
    std::vector<float> result;
    result.reserve(patches.PointsInCol() * patches.PointsInRow() * 3);

    for (int row = 0; row < patches.PointsInRow(); row++) {
        for (int col = 0; col < patches.PointsInCol(); col++) {
            Entity cp = patches.GetPoint(row, col);
            Position pos = coordinator->GetComponent<Position>(cp);

            result.push_back(pos.GetX());
            result.push_back(pos.GetY());
            result.push_back(pos.GetZ());
        }
    }

    return result;
}


std::vector<uint32_t> C0PatchesSystem::GeneratePolygonIndices(const C0SurfacePatches &patches) const
{
    std::vector<uint32_t> result;
    result.reserve(patches.PointsInCol() * patches.PointsInRow() * 2 + patches.PointsInRow()*2);

    for (int row = 0; row < patches.PointsInRow(); row++) {
        for (int col = 0; col < patches.PointsInCol(); col++) {
            result.push_back(row * patches.PointsInCol() + col);
        }

        result.push_back(std::numeric_limits<uint32_t>::max());
    }

    for (int row = 0; row < patches.PointsInCol(); row++) {
        for (int col = 0; col < patches.PointsInRow(); col++) {
            result.push_back(col * patches.PointsInCol() + row);
        }

        result.push_back(std::numeric_limits<uint32_t>::max());
    }

    return result;
}
