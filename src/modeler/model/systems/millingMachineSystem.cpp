#include <CAD_modeler/model/systems/millingMachineSystem.hpp>

#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>
#include <CAD_modeler/model/systems/meshLoader.hpp>

#include <CAD_modeler/model/components/MillingMachinePath.hpp>
#include <CAD_modeler/model/components/scale.hpp>

#include <CAD_modeler/utilities/linePlotter.hpp>
#include <CAD_modeler/utilities/line.hpp>

#include <ecs/coordinator.hpp>

#include <algebra/vec3.hpp>

#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <limits>


void MillingMachineSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<MillingMachineSystem>();

    coordinator.RegisterComponent<MillingMachinePath>();
    coordinator.RegisterComponent<Mesh>();
    coordinator.RegisterComponent<Scale>();
    coordinator.RegisterComponent<MillingCutter>();

    coordinator.RegisterRequiredComponent<MillingMachineSystem, MillingMachinePath>();
    coordinator.RegisterRequiredComponent<MillingMachineSystem, Mesh>();
}


void MillingMachineSystem::Init(const int xResolution, const int zResolution)
{
    glGenTextures(1, &heightmap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    textureData.resize(xResolution * zResolution);
    std::ranges::fill(textureData, initMaterialThickness);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R32F, xResolution, zResolution, 0, GL_RED, GL_FLOAT, textureData.data());

    material.Update(
        GenerateVertices(),
        GenerateIndices()
    );

    millingCutter = coordinator->CreateEntity();

    const MeshLoader loader;
    const Mesh millingCutterMesh = loader.LoadPositionsFromObj("../../models/millingCutter.obj");

    coordinator->AddComponent<Mesh>(millingCutter, millingCutterMesh);
    coordinator->AddComponent<Position>(millingCutter, Position());
    coordinator->AddComponent<Scale>(millingCutter, Scale(0.05f));

    heightMapXResolution = xResolution;
    heightMapZResolution = zResolution;
}


void MillingMachineSystem::AddPaths(MillingMachinePath &&paths, const MillingCutter& cutter)
{
    const Entity pathsEntity = coordinator->CreateEntity();

    Mesh pathsMesh;
    pathsMesh.Update(
        GeneratePathsVertices(paths),
        GeneratePathsIndices(paths)
    );

    coordinator->AddComponent(pathsEntity, std::move(pathsMesh));
    coordinator->AddComponent(pathsEntity, std::move(paths));

    coordinator->SetComponent<Position>(millingCutter, paths.commands[0].destination);
    coordinator->AddComponent<Scale>(millingCutter, Scale(1.f/cutter.radius));
    coordinator->AddComponent<MillingCutter>(millingCutter, cutter);
}


void MillingMachineSystem::SetMaterialResolution(const int xRes, const int zRes)
{
    heightMapXResolution = xRes;
    heightMapZResolution = zRes;

    textureData.resize(xRes * zRes);
    ResetMaterial();
}


void MillingMachineSystem::SetMaterialSize(const float xLen, const float zLen)
{
    heightMapXLen = xLen;
    heightMapZLen = zLen;

    mainHeightMapCorner.X() = -xLen / 2.f;
    mainHeightMapCorner.Z() = -zLen / 2.f;

    material.Update(
        GenerateVertices(),
        GenerateIndices()
    );
}


void MillingMachineSystem::SetInitMaterialThickness(const float thickness)
{
    initMaterialThickness = thickness;
    ResetMaterial();
}


void MillingMachineSystem::SetBaseLevel(const float level)
{
    mainHeightMapCorner.Y() = level;

    material.Update(
        GenerateVertices(),
        GenerateIndices()
    );
}


void MillingMachineSystem::ResetMaterial()
{
    std::ranges::fill(textureData, initMaterialThickness);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R32F, heightMapXResolution, heightMapZResolution, 0, GL_RED, GL_FLOAT, textureData.data());
}


std::optional<MillingCutter> MillingMachineSystem::GetMiliingCutter() const
{
    if (entities.empty())
        return std::nullopt;

    return coordinator->GetComponent<MillingCutter>(millingCutter);
}


void MillingMachineSystem::Update(const double dt)
{
    if (!cutterRuns || entities.empty())
        return;

    const auto&[commands] = coordinator->GetComponent<MillingMachinePath>(*entities.begin());
    auto cutterPos = coordinator->GetComponent<Position>(millingCutter);
    float dist = dt * cutterSpeed;

    do {
        auto const& actDest = commands[actCommand].destination;
        const float remainingCommandDist = alg::Distance(actDest.vec, cutterPos.vec);

        if (remainingCommandDist > dist) {
            auto const& prevDest = commands[actCommand - 1].destination;
            const float traveledDist = alg::Distance(prevDest.vec, cutterPos.vec);
            const alg::Vec3 newCutterPos = prevDest.vec + (traveledDist + dist) * (actDest.vec - prevDest.vec).Normalize();

            MillSection(cutterPos, newCutterPos);

            coordinator->SetComponent<Position>(millingCutter, newCutterPos);
            return;
        }

        MillSection(cutterPos, actDest.vec);
        cutterPos = actDest;
        actCommand++;
        dist -= remainingCommandDist;
    } while (actCommand < commands.size());

    StopMachine();
    actCommand = 1;
    coordinator->SetComponent<Position>(millingCutter, commands[0].destination);
}


void MillingMachineSystem::Render(const alg::Mat4x4& view, const alg::Mat4x4& perspective, const alg::Vec3& camPos)
{
    const auto& shaderRepo = ShaderRepository::GetInstance();
    const auto& shader = shaderRepo.GetMillingShader();

    shader.Use();
    glBindTexture(GL_TEXTURE_2D, heightmap);
    shader.SetCameraPosition(camPos);

    const auto cameraMtx = perspective * view;

    shader.SetHeightMapZLen(heightMapZLen);
    shader.SetHeightMapXLen(heightMapXLen);
    shader.SetMainHeightmapCorner(mainHeightMapCorner);
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

    auto const& pos = coordinator->GetComponent<Position>(millingCutter);
    auto const& scale = coordinator->GetComponent<Scale>(millingCutter);

    stdShader.SetMVP(cameraMtx * pos.TranslationMatrix() * scale.ScaleMatrix());
    stdShader.SetColor(alg::Vec4(1.f));
    auto const& cutterMesh = coordinator->GetComponent<Mesh>(millingCutter);
    cutterMesh.Use();

    glDrawElements(GL_TRIANGLES, cutterMesh.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);
}


void MillingMachineSystem::MillSection(const Position& oldCutterPos, const Position& newCutterPos)
{
    // Step 1 Mill around starting position
    const auto& cutterParams = coordinator->GetComponent<MillingCutter>(millingCutter);

    const float pixelXLen = heightMapXLen / static_cast<float>(heightMapXResolution);
    const float pixelZLen = heightMapZLen / static_cast<float>(heightMapZResolution);

    auto diff = oldCutterPos.vec - mainHeightMapCorner;
    const int oldPosPixelX = static_cast<int>(std::round(diff.X() / pixelXLen));
    const int oldPosPixelZ = static_cast<int>(std::round(diff.Z() / pixelZLen));

    diff = newCutterPos.vec - mainHeightMapCorner;
    const int newPosPixelX = static_cast<int>(std::round(diff.X() / pixelXLen));
    const int newPosPixelZ = static_cast<int>(std::round(diff.Z() / pixelZLen));

    const int radiusInPixelsX = static_cast<int>(std::ceil(cutterParams.radius / pixelXLen));
    const int radiusInPixelsZ = static_cast<int>(std::ceil(cutterParams.radius / pixelZLen));
    const int startPixelX = oldPosPixelX - radiusInPixelsX;
    const int startPixelZ = oldPosPixelZ - radiusInPixelsZ;

    for (int row = startPixelX; row <= startPixelX + 2*radiusInPixelsX; row++) {
        for (int col = startPixelZ; col <= startPixelZ + 2*radiusInPixelsZ; col++) {
            if (row < 0 || row >= heightMapZResolution || col < 0 || col >= heightMapXResolution)
                continue;

            const float globalX = static_cast<float>(row) * pixelXLen + mainHeightMapCorner.X();
            const float globalZ = static_cast<float>(col) * pixelZLen + mainHeightMapCorner.Z();

            float cutterY = CutterY(cutterParams, oldCutterPos, globalX, globalZ);

            textureData[col*heightMapXResolution + row] = std::min(textureData[col*heightMapXResolution + row], cutterY);
        }
    }

    // Step 2: Milling along the whole line
    const auto points = LinePlotter::DeterminePlotLines(
        oldPosPixelX, oldPosPixelZ,
        newPosPixelX, newPosPixelZ,
        2*std::max(radiusInPixelsX, radiusInPixelsZ)
    );

    const Line line = Line::FromTwoPoints(oldCutterPos.vec, newCutterPos.vec);

    for (auto const& point : points) {
        const int col = point.Y();
        const int row = point.X();

        if (row < 0 || row >= heightMapZResolution || col < 0 || col >= heightMapXResolution)
            continue;

        const float globalX = static_cast<float>(row) * pixelXLen + mainHeightMapCorner.X();
        const float globalZ = static_cast<float>(col) * pixelZLen + mainHeightMapCorner.Z();

        auto cutterPos = line.ProjectPointToLine(alg::Vec3(globalX, 0.f, globalZ));

        float cutterY = CutterY(cutterParams, cutterPos, globalX, globalZ);

        textureData[col*heightMapXResolution + row] = std::min(textureData[col*heightMapXResolution + row], cutterY);
    }

    glBindTexture(GL_TEXTURE_2D, heightmap);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R32F, heightMapXResolution, heightMapZResolution, 0, GL_RED, GL_FLOAT, textureData.data());
}


float MillingMachineSystem::CutterY(const MillingCutter& cutter, const Position& cutterPos, float x, float z)
{
    float diffX = x - cutterPos.GetX();
    float diffZ = z - cutterPos.GetZ();

    float lenSq = diffX*diffX + diffZ*diffZ;

    const float radiusSq = cutter.radius * cutter.radius;
    if (lenSq > radiusSq)
        return std::numeric_limits<float>::infinity();

    switch (cutter.type) {
        case MillingCutter::Type::Flat:
            return cutterPos.GetY();

        case MillingCutter::Type::Round:
            return cutter.radius - std::sqrt(radiusSq - lenSq) + cutterPos.GetY();

        default:
            throw std::runtime_error("Unknown cutter type");
    }
}


float MillingMachineSystem::UpdateHeightMap(const int row, const int col, const float cutterY, const float cutterHeight, const bool pathToDown)
{
    const float diff = std::max(textureData[col*heightMapXResolution + row] - cutterY, 0.f);
    if (diff > 0.0f) {
        const float value = textureData[col*heightMapXResolution + row] - diff;
        textureData[col*heightMapXResolution + row] = value;
    }

    if (textureData[col*heightMapXResolution + row] - mainHeightMapCorner.Y() < 0.f) {
        auto const& path = coordinator->GetComponent<MillingMachinePath>(*entities.begin());
        const auto commandId = path.commands[actCommand].id;
        millingWarnings.AddWarning(commandId, MillingWarningsRepo::MillingUnderTheBase);
    }

    if (diff > cutterHeight) {
        auto const& path = coordinator->GetComponent<MillingMachinePath>(*entities.begin());
        const auto commandId = path.commands[actCommand].id;
        millingWarnings.AddWarning(commandId, MillingWarningsRepo::MillingTooDeep);
    }

    if (pathToDown && diff > 0.f) {
        auto const& path = coordinator->GetComponent<MillingMachinePath>(*entities.begin());
        const auto commandId = path.commands[actCommand].id;
        millingWarnings.AddWarning(commandId, MillingWarningsRepo::MillingStraightDown);
    }

    return diff;
}


std::vector<float> MillingMachineSystem::GenerateVertices()
{
    std::vector<float> result;
    result.reserve(12);

    result.push_back(mainHeightMapCorner.X());
    result.push_back(mainHeightMapCorner.Y());
    result.push_back(mainHeightMapCorner.Z());

    result.push_back(mainHeightMapCorner.X() + heightMapXLen);
    result.push_back(mainHeightMapCorner.Y());
    result.push_back(mainHeightMapCorner.Z());

    result.push_back(mainHeightMapCorner.X());
    result.push_back(mainHeightMapCorner.Y());
    result.push_back(mainHeightMapCorner.Z() + heightMapZLen);

    result.push_back(mainHeightMapCorner.X() + heightMapXLen);
    result.push_back(mainHeightMapCorner.Y());
    result.push_back(mainHeightMapCorner.Z() + heightMapZLen);

    return result;
}


std::vector<uint32_t> MillingMachineSystem::GenerateIndices()
{
    return {
        0, 1, 2, 3
    };
}


std::vector<float> MillingMachineSystem::GeneratePathsVertices(const MillingMachinePath &paths)
{
    std::vector<float> result;
    result.reserve(paths.commands.size() * alg::Vec3::dim);

    for (const auto& command : paths.commands) {
        result.push_back(command.destination.GetX());
        result.push_back(command.destination.GetY());
        result.push_back(command.destination.GetZ());
    }

    return result;
}


std::vector<uint32_t> MillingMachineSystem::GeneratePathsIndices(const MillingMachinePath &paths)
{
    std::vector<std::uint32_t> result;
    result.reserve(paths.commands.size());

    for (uint32_t i = 0; i < paths.commands.size(); ++i) {
        result.push_back(i);
    }

    return result;
}
