#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>

#include "CAD_modeler/model/components/patchesDensity.hpp"
#include "CAD_modeler/model/components/curveControlPoints.hpp"
#include "CAD_modeler/model/components/mesh.hpp"

#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"

#include <CAD_modeler/utilities/setIntersection.hpp>


void C0PatchesSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C0PatchesSystem>();

    coordinator.RegisterRequiredComponent<C0PatchesSystem, C0Patches>();
    coordinator.RegisterRequiredComponent<C0PatchesSystem, PatchesDensity>();
    coordinator.RegisterRequiredComponent<C0PatchesSystem, Mesh>();
}


void C0PatchesSystem::ShowBezierNet(Entity surface)
{
    auto const& patches = coordinator->GetComponent<C0Patches>(surface);
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    netSystem->AddControlPointsNet(surface, patches);
}


void C0PatchesSystem::Render(const alg::Mat4x4& cameraMtx) const
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();

    auto const& shader = shaderRepo->GetBezierSurfaceShader();

    UpdateEntities();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    glPatchParameteri(GL_PATCH_VERTICES, 16);

    for (auto const entity: entities) {
        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        auto const& density = coordinator->GetComponent<PatchesDensity>(entity);
        float v[] = {5.f, 64.f, 64.f, 64.f};
        v[0] = static_cast<float>(density.GetDensity());
        glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, v);

	    glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void C0PatchesSystem::UpdateEntities() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    auto toUpdate = intersect(toUpdateSystem->GetEntities(), entities);

    for (auto entity: toUpdate) {
        auto const& patches = coordinator->GetComponent<C0Patches>(entity);

        UpdateMesh(entity, patches);

        if (netSystem->HasControlPointsNet(entity))
            netSystem->Update(entity, patches);

        toUpdateSystem->Unmark(entity);
    }
}


void C0PatchesSystem::UpdateMesh(Entity surface, const C0Patches& patches) const
{
    coordinator->EditComponent<Mesh>(surface,
        [surface, &patches, this](Mesh& mesh) {
            mesh.Update(
                GenerateVertices(patches),
                GenerateIndices(patches)
            );
        }
    );
}


std::vector<float> C0PatchesSystem::GenerateVertices(const C0Patches& patches) const
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


std::vector<uint32_t> C0PatchesSystem::GenerateIndices(const C0Patches& patches) const
{
    std::vector<uint32_t> result;
    result.reserve(patches.PatchesInRow() * patches.PatchesInCol() * C0Patches::PointsInPatch * 2);

    for (int patchRow=0; patchRow < patches.PatchesInRow(); patchRow++) {
        for (int patchCol=0; patchCol < patches.PatchesInCol(); patchCol++) {
            for (int rowInPatch=0; rowInPatch < C0Patches::RowsInPatch; rowInPatch++) {
                for (int colInPatch=0; colInPatch < C0Patches::ColsInPatch; colInPatch++) {

                    int globCol = patchCol * (C0Patches::ColsInPatch - 1) + colInPatch;
                    int globRow = patchRow * (C0Patches::RowsInPatch - 1) + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }

            for (int colInPatch=0; colInPatch < C0Patches::ColsInPatch; colInPatch++) {
                for (int rowInPatch=0; rowInPatch < C0Patches::RowsInPatch; rowInPatch++) {

                    int globCol = patchCol * (C0Patches::ColsInPatch - 1) + colInPatch;
                    int globRow = patchRow * (C0Patches::RowsInPatch - 1) + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }
        }
    }

    return result;
}
