#include <CAD_modeler/model/systems/c0PatchesTrianglesRenderSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/meshC0PatchesTriangles.hpp>
#include <CAD_modeler/model/components/trianglesPatchesDensityLevel.hpp>


void C0PatchesTrianglesRenderSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C0PatchesTrianglesRenderSystem>();

    coordinator.RegisterComponent<MeshC0PatchesTriangles>();
    coordinator.RegisterComponent<TrianglesPatchesDensityLevel>();
}


void C0PatchesTrianglesRenderSystem::AddSurface(const Entity entity, const float densityLevel)
{
    entities.insert(entity);

    auto const& patches = coordinator->GetComponent<C0Patches>(entity);
    MeshC0PatchesTriangles mesh;

    mesh.Update(
        GenerateVertices(patches),
        GenerateIndices(patches)
    );

    coordinator->AddComponent(entity, mesh);
    coordinator->AddComponent(entity, TrianglesPatchesDensityLevel(densityLevel));
}


void C0PatchesTrianglesRenderSystem::RemoveSurface(const Entity entity)
{
    entities.erase(entity);
    coordinator->DeleteComponent<MeshC0PatchesTriangles>(entity);
    coordinator->DeleteComponent<TrianglesPatchesDensityLevel>(entity);
}


void C0PatchesTrianglesRenderSystem::Render(const alg::Mat4x4 &cameraMtx)
{
    if (entities.empty())
        return;

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    glPatchParameteri(GL_PATCH_VERTICES, 16);

    for (const Entity entity : entities) {
        auto const& mesh = coordinator->GetComponent<MeshC0PatchesTriangles>(entity);
        const auto& [value] = coordinator->GetComponent<TrianglesPatchesDensityLevel>(entity);

        mesh.Use();

        shader.SetTesselationLevel(value);
        glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);
    }
}


std::vector<float> C0PatchesTrianglesRenderSystem::GenerateVertices(const C0Patches &patches) const
{
    std::vector<float> result;
    result.reserve(patches.PointsCnt() * 3);

    for (size_t col=0; col < patches.PointsInCol(); col++) {
        for (size_t row=0; row < patches.PointsInRow(); row++) {
            const Entity point = patches.GetPoint(row, col);

            auto const& pos = coordinator->GetComponent<Position>(point);

            result.push_back(pos.GetX());
            result.push_back(pos.GetY());
            result.push_back(pos.GetZ());
        }
    }

    return result;
}


std::vector<uint32_t> C0PatchesTrianglesRenderSystem::GenerateIndices(const C0Patches &patches) const
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
        }
    }

    return result;
}
