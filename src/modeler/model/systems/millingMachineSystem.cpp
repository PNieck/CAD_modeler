#include <CAD_modeler/model/systems/millingMachineSystem.hpp>

#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>

#include <ecs/coordinator.hpp>

#include <glad/glad.h>


void MillingMachineSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<MillingMachineSystem>();
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


void MillingMachineSystem::Render(const alg::Mat4x4& view, const alg::Mat4x4& perspective)
{
    const auto& shader = ShaderRepository::GetInstance().GetMillingShader();

    shader.Use();
    glBindTexture(GL_TEXTURE_2D, heightmap);
    shader.SetColor(alg::Vec4(1.0f));

    shader.SetHeightMapLength(2.f);
    shader.SetHeightMapWidth(2.f);
    shader.SetMainHeightmapCorner(alg::Vec3(-1.f, 0.f, -1.f));
    shader.SetMVP(perspective * view);
    //shader.SetViewMtx(view);
    shader.SetProjection00(perspective(0, 0));

    material.Use();
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawElements(GL_PATCHES, material.GetElementsCnt(), GL_UNSIGNED_INT, 0);
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
