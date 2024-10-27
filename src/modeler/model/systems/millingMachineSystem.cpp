#include <CAD_modeler/model/systems/millingMachineSystem.hpp>

#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>

#include <CAD_modeler/model/components/millingMachinePaths.hpp>

#include <ecs/coordinator.hpp>

#include <glad/glad.h>


void MillingMachineSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<MillingMachineSystem>();

    coordinator.RegisterComponent<MillingMachinePaths>();
    coordinator.RegisterComponent<Mesh>();

    coordinator.RegisterRequiredComponent<MillingMachineSystem, MillingMachinePaths>();
    coordinator.RegisterRequiredComponent<MillingMachineSystem, Mesh>();
}


void MillingMachineSystem::CreateMaterial(const int xResolution, const int yResolution)
{
    glGenTextures(1, &heightmap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    std::vector<float> data(xResolution * yResolution);

    for (int row=0; row < xResolution; row++) {
        for (int col=0; col < yResolution; col++) {
            data[row*xResolution + col] = 0.2f * (std::sin((float)row / (float)xResolution * 10.f) + std::cos((float)col / (float)yResolution * 10.f));
        }
    }

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R32F, xResolution, yResolution, 0, GL_RED, GL_FLOAT, data.data());

    material.Update(
        GenerateVertices(),
        GenerateIndices()
    );
}


void MillingMachineSystem::AddPaths(MillingMachinePaths &&paths)
{
    const Entity pathsEntity = coordinator->CreateEntity();

    Mesh pathsMesh;
    pathsMesh.Update(
        GeneratePathsVertices(paths),
        GeneratePathsIndices(paths)
    );

    coordinator->AddComponent(pathsEntity, std::move(pathsMesh));
    coordinator->AddComponent(pathsEntity, std::move(paths));
}


void MillingMachineSystem::Render(const alg::Mat4x4& view, const alg::Mat4x4& perspective)
{
    const auto& shaderRepo = ShaderRepository::GetInstance();
    const auto& shader = shaderRepo.GetMillingShader();

    shader.Use();
    glBindTexture(GL_TEXTURE_2D, heightmap);
    shader.SetColor(alg::Vec4(1.0f));

    auto cameraMtx = perspective * view;

    shader.SetHeightMapLength(2.f);
    shader.SetHeightMapWidth(2.f);
    shader.SetMainHeightmapCorner(alg::Vec3(-1.f, 0.f, -1.f));
    shader.SetMVP(cameraMtx);
    shader.SetProjection00(perspective(0, 0));

    material.Use();
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawElements(GL_PATCHES, material.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);

    if (entities.empty())
        return;

    auto const& stdShader = shaderRepo.GetStdShader();

    stdShader.Use();

    // Neon green
    stdShader.SetColor(alg::Vec4(23.f/255.f, 227.f/255.f, 33.f/255.f, 1.f));
    shader.SetMVP(cameraMtx);

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);
    }
}


std::vector<float> MillingMachineSystem::GenerateVertices()
{
    return {
        -1.f, 0.f, -1.f,
        1.f, 0.f, -1.f,
        -1.f, 0.f, 1.f,
        1.f, 0.f, 1.f
    };
}


std::vector<uint32_t> MillingMachineSystem::GenerateIndices()
{
    return {
        0, 1, 2, 3
    };
}


std::vector<float> MillingMachineSystem::GeneratePathsVertices(const MillingMachinePaths &paths)
{
    std::vector<float> result;
    result.reserve(paths.GetSize() * alg::Vec3::dim);

    for (const auto& pos : paths.GetPositions()) {
        result.push_back(pos.GetX());
        result.push_back(pos.GetY());
        result.push_back(pos.GetZ());
    }

    return result;
}


std::vector<uint32_t> MillingMachineSystem::GeneratePathsIndices(const MillingMachinePaths &paths)
{
    std::vector<std::uint32_t> result;
    result.reserve(paths.GetSize());

    for (uint32_t i = 0; i < paths.GetSize(); ++i) {
        result.push_back(i);
    }

    return result;
}



