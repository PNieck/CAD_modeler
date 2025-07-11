#include <CAD_modeler/model/systems/millingMachineSystem.hpp>

#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>
#include <CAD_modeler/model/systems/meshLoader.hpp>

#include <CAD_modeler/model/components/MillingMachinePath.hpp>
#include <CAD_modeler/model/components/scale.hpp>

#include <CAD_modeler/utilities/linePlotter.hpp>
#include <CAD_modeler/utilities/lineSegment.hpp>

#include <ecs/coordinator.hpp>

#include <algebra/vec3.hpp>

#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <stack>
#include <thread>
#include <cassert>


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


MillingMachineSystem::MillingMachineSystem():
    heightmap(0, 0, nullptr, Texture2D::Red32BitFloat, Texture2D::Red),
    instantWorker([this](std::stop_token token) {
        InstantMillingThreadFunc(token);
    })
{
}


void MillingMachineSystem::Init(const int xResolution, const int zResolution)
{
    textureData.resize(xResolution * zResolution);
    std::ranges::fill(textureData, initMaterialThickness);

    heightmap.ChangeSize(xResolution, zResolution, textureData.data(), Texture2D::Red);

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
}


void MillingMachineSystem::AddPaths(MillingMachinePath &&paths, const MillingCutter& cutter) const
{
    const Entity pathsEntity = coordinator->CreateEntity();

    if (!entities.empty()) {
        std::stack<Entity> entitiesToDelete;
        for (const auto entity: entities)
            entitiesToDelete.push(entity);

        while (!entitiesToDelete.empty()) {
            coordinator->DestroyEntity(entitiesToDelete.top());
            entitiesToDelete.pop();
        }
    }

    Mesh pathsMesh;
    pathsMesh.Update(
        GeneratePathsVertices(paths),
        GeneratePathsIndices(paths)
    );

    coordinator->AddComponent(pathsEntity, std::move(pathsMesh));
    coordinator->AddComponent(pathsEntity, std::move(paths));

    coordinator->SetComponent<Position>(millingCutter, paths.commands[0].destination);
    coordinator->AddComponent<Scale>(millingCutter, Scale(1.f/cutter.radius));

    if (coordinator->HasComponent<MillingCutter>(millingCutter))
        coordinator->SetComponent(millingCutter, cutter);
    else
        coordinator->AddComponent<MillingCutter>(millingCutter, cutter);
}


void MillingMachineSystem::StartInstantMilling()
{
    instantWorker.StartWork();
}


void MillingMachineSystem::StopInstantMilling()
{
    instantWorker.JoinWorker();
}


void MillingMachineSystem::SetMaterialResolution(const int xRes, const int zRes)
{
    textureData.resize(xRes * zRes);
    std::ranges::fill(textureData, initMaterialThickness);
    heightmap.ChangeSize(xRes, zRes, textureData.data(), Texture2D::Red);
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

    heightmap.Update(textureData.data(), Texture2D::Red);
}


void MillingMachineSystem::ResetSimulation()
{
    ResetMaterial();
    actCommand = 1;
    const auto&[commands] = coordinator->GetComponent<MillingMachinePath>(*entities.begin());
    coordinator->SetComponent<Position>(millingCutter, commands[0].destination);
    millingWarnings.Clear();
}


std::optional<MillingCutter> MillingMachineSystem::GetMillingCutter() const
{
    if (entities.empty())
        return std::nullopt;

    return coordinator->GetComponent<MillingCutter>(millingCutter);
}


void MillingMachineSystem::SetCutterHeight(float height) const
{
    coordinator->EditComponent<MillingCutter>(millingCutter,
        [height] (MillingCutter& cutter) {
            cutter.height = height;
        }
    );
}


void MillingMachineSystem::Update(const double dt)
{
    if (InstantMillingRuns()) {
        if (instantWorker.WaitsForJoin()) {
            instantWorker.JoinWorker();
            heightmap.Update(textureData.data(), Texture2D::Red);
        }
    }

    if (!cutterRuns || entities.empty())
        return;

    const auto&[commands] = coordinator->GetComponent<MillingMachinePath>(*entities.begin());
    auto cutterPos = coordinator->GetComponent<Position>(millingCutter);
    float dist = static_cast<float>(dt) * cutterSpeed;

    do {
        auto const& actDest = commands[actCommand].destination;
        const float remainingCommandDist = alg::Distance(actDest.vec, cutterPos.vec);

        if (remainingCommandDist > dist) {
            auto const& prevDest = commands[actCommand - 1].destination;
            const float traveledDist = alg::Distance(prevDest.vec, cutterPos.vec);
            const alg::Vec3 newCutterPos = prevDest.vec + (traveledDist + dist) * (actDest.vec - prevDest.vec).Normalize();

            MillSection(cutterPos, newCutterPos);

            coordinator->SetComponent<Position>(millingCutter, newCutterPos);

            heightmap.Update(textureData.data(), Texture2D::Red);
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

    heightmap.Update(textureData.data(), Texture2D::Red);
}


void MillingMachineSystem::Render(const alg::Mat4x4& view, const alg::Mat4x4& perspective, const alg::Vec3& camPos)
{
    const auto& shaderRepo = ShaderRepository::GetInstance();
    const auto& shader = shaderRepo.GetMillingShader();

    shader.Use();
    heightmap.Use();
    shader.SetCameraPosition(camPos);

    const auto cameraMtx = perspective * view;

    shader.SetHeightMapZLen(heightMapZLen);
    shader.SetHeightMapXLen(heightMapXLen);
    shader.SetMainHeightmapCorner(mainHeightMapCorner);
    shader.SetMVP(cameraMtx);

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


void MillingMachineSystem::InstantMillingThreadFunc(std::stop_token stoken)
{
    if (entities.empty())
        return;

    const auto&[commands] = coordinator->GetComponent<MillingMachinePath>(*entities.begin());

    while (actCommand < commands.size() && !stoken.stop_requested()) {
        const auto& prevDest = commands[actCommand - 1].destination;
        const auto& actDest = commands[actCommand].destination;

        MillSection(prevDest, actDest);
        actCommand++;
    }

    actCommand = 1;
}


void MillingMachineSystem::MillSection(const Position& oldCutterPos, const Position& newCutterPos)
{
    // Step 1 Mill around starting position
    const auto& cutterParams = coordinator->GetComponent<MillingCutter>(millingCutter);

    const float pixelXLen = heightMapXLen / static_cast<float>(heightmap.GetWidth());
    const float pixelZLen = heightMapZLen / static_cast<float>(heightmap.GetHeight());

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

    const float crossLen = Cross((newCutterPos.vec - oldCutterPos.vec).Normalize(), alg::Vec3(0.f, 1.f, 0.f)).LengthSquared();
    const bool straightDown = crossLen < 1e-5;

    for (int row = startPixelX; row <= startPixelX + 2*radiusInPixelsX; row++) {
        for (int col = startPixelZ; col <= startPixelZ + 2*radiusInPixelsZ; col++) {
            if (row < 0 || row >= heightmap.GetHeight() || col < 0 || col >= heightmap.GetWidth())
                continue;

            const float globalX = static_cast<float>(row) * pixelXLen + mainHeightMapCorner.X();
            const float globalZ = static_cast<float>(col) * pixelZLen + mainHeightMapCorner.Z();

            const float cutterY = CutterY(cutterParams, oldCutterPos, globalX, globalZ);

            UpdateHeightMap(row, col, cutterY, cutterParams.height, straightDown);
        }
    }

    // Step 2: Milling along the whole line
    const auto [points, lineTyp] = LinePlotter::DeterminePlotLines(
        oldPosPixelX, oldPosPixelZ,
        newPosPixelX, newPosPixelZ
    );

    const LineSegment lineSegment(oldCutterPos.vec, newCutterPos.vec);

    for (auto const& point : points) {
        int cutterRadius;

        switch (lineTyp) {
            case LinePlotter::LineType::Low:
                cutterRadius = radiusInPixelsX;
                break;

            case LinePlotter::LineType::High:
                cutterRadius = radiusInPixelsZ;
                break;

            default:
                throw std::runtime_error("Unknown line type");
        }

        for (int i = -cutterRadius; i <= cutterRadius; i++) {
            const int row = lineTyp == LinePlotter::LineType::High ? point.X() + i : point.X();
            const int col = lineTyp == LinePlotter::LineType::Low ? point.Y() + i : point.Y();

            if (row < 0 || row >= heightmap.GetHeight() || col < 0 || col >= heightmap.GetWidth())
                continue;

            const float globalX = static_cast<float>(row) * pixelXLen + mainHeightMapCorner.X();
            const float globalZ = static_cast<float>(col) * pixelZLen + mainHeightMapCorner.Z();

            const auto cutterPos = lineSegment.NearestPoint(alg::Vec3(globalX, 0.f, globalZ));
            const float cutterY = CutterY(cutterParams, cutterPos, globalX, globalZ);

            UpdateHeightMap(row, col, cutterY, cutterParams.height, straightDown);
        }
    }
}


float MillingMachineSystem::CutterY(const MillingCutter& cutter, const Position& cutterPos, float x, float z)
{
    const float diffX = x - cutterPos.GetX();
    const float diffZ = z - cutterPos.GetZ();

    const float lenSq = diffX*diffX + diffZ*diffZ;

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
    const float diff = std::max(textureData[col*heightmap.GetWidth() + row] - cutterY, 0.f);
    if (diff > 0.0f) {
        const float value = textureData[col*heightmap.GetWidth() + row] - diff;
        textureData[col*heightmap.GetWidth() + row] = value;
    }

    if (textureData[col*heightmap.GetWidth() + row] - mainHeightMapCorner.Y() < 0.f) {
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
    const int pointsInX = heightmap.GetWidth() / 50 + 1;
    const int pointsInZ = heightmap.GetHeight() / 50 + 1;

    result.reserve(pointsInX * pointsInZ * alg::Vec3::dim);

    for (int row = 0; row < pointsInZ; row++) {
        for (int col = 0; col < pointsInX; col++) {
            const float x = heightMapXLen / static_cast<float>((pointsInX - 1)) * col;
            const float z = heightMapZLen / static_cast<float>((pointsInZ - 1)) * row;

            result.push_back(mainHeightMapCorner.X() + x);
            result.push_back(mainHeightMapCorner.Y());
            result.push_back(mainHeightMapCorner.Z() + z);
        }
    }

    return result;
}


std::vector<uint32_t> MillingMachineSystem::GenerateIndices()
{
    std::vector<uint32_t> result;

    const int pointsInX = heightmap.GetWidth() / 50 + 1;
    const int pointsInZ = heightmap.GetHeight() / 50 + 1;

    for (int row = 0; row < pointsInZ - 1; row ++) {
        for (int col = 0; col < pointsInX - 1; col++) {
            result.push_back(row + pointsInZ * col + pointsInZ);
            result.push_back(row + pointsInZ * col);

            result.push_back(row + pointsInZ * col + pointsInZ + 1);
            result.push_back(row + pointsInZ * col + 1);

        }
    }

    return result;
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
