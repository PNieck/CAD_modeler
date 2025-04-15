#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>

#include "CAD_modeler/model/components/patchesDensity.hpp"
#include "CAD_modeler/model/components/curveControlPoints.hpp"
#include "CAD_modeler/model/components/mesh.hpp"

#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsRegistrySystem.hpp"
#include "CAD_modeler/model/systems/c0PatchesSystem/singleC0Patch.hpp"
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


Position C0PatchesSystem::PointOnPatches(const C0Patches &patches, float u, float v) const
{
    CheckUVDomain(patches, u, v);

    const SingleC0Patch p(*coordinator, patches, u, v);

    NormalizeUV(patches, u, v);

    const auto bu = CubicBernsteinPolynomial(u);
    const auto bv = CubicBernsteinPolynomial(v);

    // Result of bu*p
    const auto a0 = bu.X()*p.Point(0, 0) + bu.Y()*p.Point(1, 0) + bu.Z()*p.Point(2, 0) + bu.W()*p.Point(3, 0);
    const auto a1 = bu.X()*p.Point(0, 1) + bu.Y()*p.Point(1, 1) + bu.Z()*p.Point(2, 1) + bu.W()*p.Point(3, 1);
    const auto a2 = bu.X()*p.Point(0, 2) + bu.Y()*p.Point(1, 2) + bu.Z()*p.Point(2, 2) + bu.W()*p.Point(3, 2);
    const auto a3 = bu.X()*p.Point(0, 3) + bu.Y()*p.Point(1, 3) + bu.Z()*p.Point(2, 3) + bu.W()*p.Point(3, 3);

    // pos = bu * p * bv^T
    return a0*bv.X() + a1*bv.Y() + a2*bv.Z() + a3*bv.W();
}


alg::Vec3 Derivative(const float t, const alg::Vec3& a0, const alg::Vec3& a1, const alg::Vec3& a2, const alg::Vec3& a3) {
    const float oneMinusT = 1.f - t;

    return -3.f * oneMinusT * oneMinusT * a0 +
           (3.f * oneMinusT * oneMinusT - 6.f * t * oneMinusT) * a1 +
           (6.f * t * oneMinusT - 3.f * t * t) * a2 +
           3.f * t * t * a3;
}


alg::Vec3 C0PatchesSystem::PartialDerivativeU(const C0Patches &patches, float u, float v) const
{
    CheckUVDomain(patches, u, v);

    const SingleC0Patch p(*coordinator, patches, u, v);

    NormalizeUV(patches, u, v);

    const auto bv = CubicBernsteinPolynomial(v);

    // Result of bu*p
    const auto a0 = bv.X()*p.Point(0, 0) + bv.Y()*p.Point(0, 1) + bv.Z()*p.Point(0, 2) + bv.W()*p.Point(0, 3);
    const auto a1 = bv.X()*p.Point(1, 0) + bv.Y()*p.Point(1, 1) + bv.Z()*p.Point(1, 2) + bv.W()*p.Point(1, 3);
    const auto a2 = bv.X()*p.Point(2, 0) + bv.Y()*p.Point(2, 1) + bv.Z()*p.Point(2, 2) + bv.W()*p.Point(2, 3);
    const auto a3 = bv.X()*p.Point(3, 0) + bv.Y()*p.Point(3, 1) + bv.Z()*p.Point(3, 2) + bv.W()*p.Point(3, 3);

    return Derivative(u, a0, a1, a2, a3);
}


alg::Vec3 C0PatchesSystem::PartialDerivativeV(const C0Patches &patches, float u, float v) const
{
    CheckUVDomain(patches, u, v);

    const SingleC0Patch p(*coordinator, patches, u, v);

    NormalizeUV(patches, u, v);

    const auto bu = CubicBernsteinPolynomial(u);

    // Result of bu*p
    const auto a0 = bu.X()*p.Point(0, 0) + bu.Y()*p.Point(1, 0) + bu.Z()*p.Point(2, 0) + bu.W()*p.Point(3, 0);
    const auto a1 = bu.X()*p.Point(0, 1) + bu.Y()*p.Point(1, 1) + bu.Z()*p.Point(2, 1) + bu.W()*p.Point(3, 1);
    const auto a2 = bu.X()*p.Point(0, 2) + bu.Y()*p.Point(1, 2) + bu.Z()*p.Point(2, 2) + bu.W()*p.Point(3, 2);
    const auto a3 = bu.X()*p.Point(0, 3) + bu.Y()*p.Point(1, 3) + bu.Z()*p.Point(2, 3) + bu.W()*p.Point(3, 3);

    return Derivative(v, a0, a1, a2, a3);
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

    for (const auto entity: toUpdateSystem->GetEntitiesToUpdate<C0PatchesSystem>()) {
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

void C0PatchesSystem::CheckUVDomain(const C0Patches &patches, const float u, const float v)
{
    if (!(v >= 0.f && v <= MaxV(patches)) || !(u >= 0.f && u <= MaxU(patches)))
        throw std::runtime_error("Arguments outside the domain");
}


void C0PatchesSystem::NormalizeUV(const C0Patches &patches, float& u, float& v)
{
    if (u == MaxU(patches))
        u = 1.f;
    else
        u -= std::floor(u);

    if (v == MaxV(patches))
        v = 1.f;
    else
        v -= std::floor(v);
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
