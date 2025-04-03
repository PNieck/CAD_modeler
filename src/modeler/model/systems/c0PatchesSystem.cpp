#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>

#include "CAD_modeler/model/components/patchesDensity.hpp"
#include "CAD_modeler/model/components/curveControlPoints.hpp"
#include "CAD_modeler/model/components/mesh.hpp"

#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsRegistrySystem.hpp"
#include "CAD_modeler/model/systems/shaders/shaderRepository.hpp"


void C0PatchesSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C0PatchesSystem>();

    coordinator.RegisterRequiredComponent<C0PatchesSystem, C0Patches>();
    coordinator.RegisterRequiredComponent<C0PatchesSystem, PatchesDensity>();
    coordinator.RegisterRequiredComponent<C0PatchesSystem, Mesh>();
}


void C0PatchesSystem::MergeControlPoints(const Entity surface, const Entity oldCP, Entity newCP, const SystemId system)
{
    coordinator->EditComponent<C0Patches>(surface,
        [oldCP, newCP, this] (C0Patches& patches) {
            for (int row=0; row < patches.PointsInRow(); row++) {
                for (int col=0; col < patches.PointsInCol(); col++) {
                    if (patches.GetPoint(row, col) == oldCP)
                        patches.SetPoint(newCP, row, col);
                }
            }

            const HandlerId handlerId = patches.controlPointsHandlers.at(oldCP);

            if (!patches.controlPointsHandlers.contains(newCP)) {
                const auto eventHandler = coordinator->GetEventHandler<Position>(oldCP, handlerId);
                auto newEventHandlerId = coordinator->Subscribe<Position>(newCP, eventHandler);
                patches.controlPointsHandlers.insert({newCP, newEventHandlerId});
            }

            coordinator->Unsubscribe<Position>(oldCP, handlerId);
            patches.controlPointsHandlers.erase(oldCP);
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(surface);

    const auto registry = coordinator->GetSystem<ControlPointsRegistrySystem>();
    registry->UnregisterControlPoint(surface, oldCP, system);
    registry->RegisterControlPoint(surface, newCP, system);
}


void C0PatchesSystem::ShowBezierNet(const Entity surface)
{
    auto const& patches = coordinator->GetComponent<C0Patches>(surface);
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    netSystem->AddControlPointsNet(surface, patches);
}


alg::Vec4 CubicBernsteinPolynomial(const float t) {
    const float oneMinusT = 1.0f - t;

    return {
        oneMinusT * oneMinusT * oneMinusT,
        3.f * oneMinusT * oneMinusT * t,
        3.f * oneMinusT * t * t,
        t * t * t
    };
}


Position C0PatchesSystem::PointOnPatches(Entity entity, float u, float v) const
{
    auto const& patches = coordinator->GetComponent<C0Patches>(entity);

    assert(v >= 0.f && static_cast<float>(patches.PatchesInRow()) >= v);
    assert(u >= 0.f && static_cast<float>(patches.PatchesInCol()) >= u);

    const float vFloor = std::floor(v);
    const float uFloor = std::floor(u);

    int firstRow = static_cast<int>(vFloor) * 3;
    if (v == static_cast<float>(patches.PatchesInRow()))
        --firstRow;

    int firstCol = static_cast<int>(uFloor) * 3;
    if (u == static_cast<float>(patches.PatchesInCol()))
        --firstCol;

    const float vNormalized = v - vFloor;
    const float uNormalized = u - uFloor;

    const auto bu = CubicBernsteinPolynomial(uNormalized);
    const auto bv = CubicBernsteinPolynomial(vNormalized);

    const auto& p00 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow, firstCol));
    const auto& p01 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow, firstCol + 1));
    const auto& p02 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow, firstCol + 2));
    const auto& p03 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow, firstCol + 3));

    const auto& p10 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 1, firstCol));
    const auto& p11 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 1, firstCol + 1));
    const auto& p12 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 1, firstCol + 2));
    const auto& p13 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 1, firstCol + 3));

    const auto& p20 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 2, firstCol));
    const auto& p21 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 2, firstCol + 1));
    const auto& p22 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 2, firstCol + 2));
    const auto& p23 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 2, firstCol + 3));

    const auto& p30 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 3, firstCol));
    const auto& p31 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 3, firstCol + 1));
    const auto& p32 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 3, firstCol + 2));
    const auto& p33 = coordinator->GetComponent<Position>(patches.GetPoint(firstRow + 3, firstCol + 3));

    // Result of bu*p
    const auto a0 = bu.X()*p00.vec + bu.Y()*p10.vec + bu.Z()*p20.vec + bu.W()*p30.vec;
    const auto a1 = bu.X()*p01.vec + bu.Y()*p11.vec + bu.Z()*p21.vec + bu.W()*p31.vec;
    const auto a2 = bu.X()*p02.vec + bu.Y()*p12.vec + bu.Z()*p22.vec + bu.W()*p32.vec;
    const auto a3 = bu.X()*p03.vec + bu.Y()*p13.vec + bu.Z()*p23.vec + bu.W()*p33.vec;

    // pos = bu * p * bv^T
    return a0*bv.X() + a1*bv.Y() + a2*bv.Z() + a3*bv.W();
}


void C0PatchesSystem::Render(const alg::Mat4x4& cameraMtx) const
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetBezierSurfaceShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    glPatchParameteri(GL_PATCH_VERTICES, 16);

    for (auto const entity: entities) {
        const bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        auto const& density = coordinator->GetComponent<PatchesDensity>(entity);
        float v[] = {5.f, 64.f, 64.f, 64.f};
        v[0] = static_cast<float>(density.GetDensity());
        glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, v);

	    glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void C0PatchesSystem::Update() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    auto toUpdate = toUpdateSystem->GetEntitiesToUpdate<C0PatchesSystem>();

    for (const auto entity: toUpdate) {
        auto const& patches = coordinator->GetComponent<C0Patches>(entity);

        UpdateMesh(entity, patches);

        if (netSystem->HasControlPointsNet(entity))
            netSystem->Update(entity, patches);
    }

    toUpdateSystem->UnmarkAll<C0PatchesSystem>();
}


void C0PatchesSystem::UpdateMesh(const Entity surface, const C0Patches& patches) const
{
    coordinator->EditComponent<Mesh>(surface,
        [&patches, this](Mesh& mesh) {
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
            const Entity point = patches.GetPoint(row, col);

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

                    const int globCol = patchCol * (C0Patches::ColsInPatch - 1) + colInPatch;
                    const int globRow = patchRow * (C0Patches::RowsInPatch - 1) + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }

            for (int colInPatch=0; colInPatch < C0Patches::ColsInPatch; colInPatch++) {
                for (int rowInPatch=0; rowInPatch < C0Patches::RowsInPatch; rowInPatch++) {

                    const int globCol = patchCol * (C0Patches::ColsInPatch - 1) + colInPatch;
                    const int globRow = patchRow * (C0Patches::RowsInPatch - 1) + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }
        }
    }

    return result;
}
