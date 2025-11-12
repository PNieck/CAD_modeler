#include <CAD_modeler/model/systems/c2PatchesTrianglesRenderingSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/meshPatchesTriangles.hpp>
#include <CAD_modeler/model/components/trianglesPatchesDensityLevel.hpp>


void C2PatchesTrianglesRenderSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C2PatchesTrianglesRenderSystem>();

    coordinator.RegisterComponent<MeshPatchesTriangles>();
    coordinator.RegisterComponent<TrianglesPatchesDensityLevel>();
}


void C2PatchesTrianglesRenderSystem::AddSurface(const Entity entity, const float densityLevel)
{
    entities.insert(entity);

    auto const& patches = coordinator->GetComponent<C2Patches>(entity);
    MeshPatchesTriangles mesh;

    mesh.Update(
        GenerateVertices(patches),
        GenerateIndices(patches)
    );

    coordinator->AddComponent(entity, mesh);
    coordinator->AddComponent(entity, TrianglesPatchesDensityLevel(densityLevel));
}


void C2PatchesTrianglesRenderSystem::RemoveSurface(const Entity entity)
{
    entities.erase(entity);
    coordinator->DeleteComponent<MeshPatchesTriangles>(entity);
    coordinator->DeleteComponent<TrianglesPatchesDensityLevel>(entity);
}


void C2PatchesTrianglesRenderSystem::Render(const alg::Mat4x4 &cameraMtx)
{
    if (entities.empty())
        return;

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    glPatchParameteri(GL_PATCH_VERTICES, 16);

    for (const Entity entity : entities) {
        auto const& mesh = coordinator->GetComponent<MeshPatchesTriangles>(entity);
        const auto& [value] = coordinator->GetComponent<TrianglesPatchesDensityLevel>(entity);

        mesh.Use();

        shader.SetDensityLevel(value);
        glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);
    }
}


std::vector<float> C2PatchesTrianglesRenderSystem::GenerateVertices(const C2Patches &patches) const
{
    std::vector<float> result;
    result.reserve(patches.PointsCnt() * alg::Vec3::dim);

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


std::vector<uint32_t> C2PatchesTrianglesRenderSystem::GenerateIndices(const C2Patches &patches) const
{
    std::vector<uint32_t> result;
    result.reserve(patches.PatchesInRow() * patches.PatchesInCol() * C2Patches::PointsInPatch);

    for (int patchRow=0; patchRow < patches.PatchesInRow(); patchRow++) {
        for (int patchCol=0; patchCol < patches.PatchesInCol(); patchCol++) {
            for (int rowInPatch=0; rowInPatch < C2Patches::RowsInPatch; rowInPatch++) {
                for (int colInPatch=0; colInPatch < C2Patches::ColsInPatch; colInPatch++) {

                    const int globCol = patchCol + colInPatch;
                    const int globRow = patchRow + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }
        }
    }

    return result;
}
