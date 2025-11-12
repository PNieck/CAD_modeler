#include <CAD_modeler/model/systems/millingMachineSystem.hpp>

#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>
#include <CAD_modeler/model/systems/meshLoader.hpp>

#include <CAD_modeler/model/components/millingMachinePath.hpp>
#include <CAD_modeler/model/components/scale.hpp>

#include <CAD_modeler/utilities/lineDrawer.hpp>
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
    material(0, 0, 1.5f, 1.5f, 0.5f),
    instantWorker([this](std::stop_token token) {
        InstantMillingThreadFunc(token);
    })
{
}


void MillingMachineSystem::Init(const int xResolution, const int zResolution)
{
    material.SetResolution(xResolution, zResolution);

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
            material.SyncVisualization();
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

            material.SyncVisualization();
            return;
        }

        MillSection(cutterPos, actDest.vec);
        cutterPos = actDest;
        actCommand++;
        dist -= remainingCommandDist;
    } while (static_cast<size_t>(actCommand) < commands.size());

    StopMachine();
    actCommand = 1;
    coordinator->SetComponent<Position>(millingCutter, commands[0].destination);

    material.SyncVisualization();
}


void MillingMachineSystem::Render(const alg::Mat4x4& view, const alg::Mat4x4& perspective, const alg::Vec3& camPos)
{
    const auto cameraMtx = perspective * view;

    material.Render(cameraMtx, camPos);

    if (entities.empty())
        return;

    RenderPaths(cameraMtx);
    RenderCutter(cameraMtx);
}


void MillingMachineSystem::InstantMillingThreadFunc(std::stop_token stoken)
{
    if (entities.empty())
        return;

    const auto&[commands] = coordinator->GetComponent<MillingMachinePath>(*entities.begin());

    while (static_cast<size_t>(actCommand) < commands.size() && !stoken.stop_requested()) {
        const auto& prevDest = commands[actCommand - 1].destination;
        const auto& actDest = commands[actCommand].destination;

        MillSection(prevDest, actDest);
        actCommand++;
    }

    actCommand = 1;
}


void MillingMachineSystem::MillSection(const Position& oldCutterPos, const Position& newCutterPos)
{
    if (oldCutterPos.vec == newCutterPos.vec)
        return;

    // Step 1 Mill around the starting position
    const auto& cutter = coordinator->GetComponent<MillingCutter>(millingCutter);

    const float pixelXLen = material.PixelXLen();
    const float pixelZLen = material.PixelZLen();

    float diffX = oldCutterPos.GetX() - material.MinX();
    float diffZ = oldCutterPos.GetZ() - material.MinZ();
    const int oldPosPixelX = static_cast<int>(std::round(diffX / pixelXLen));
    const int oldPosPixelZ = static_cast<int>(std::round(diffZ / pixelZLen));

    diffX = newCutterPos.GetX() - material.MinX();
    diffZ = newCutterPos.GetZ() - material.MinZ();
    const int newPosPixelX = static_cast<int>(std::round(diffX / pixelXLen));
    const int newPosPixelZ = static_cast<int>(std::round(diffZ / pixelZLen));

    const int radiusInPixelsX = static_cast<int>(std::ceil(cutter.radius / pixelXLen));
    const int radiusInPixelsZ = static_cast<int>(std::ceil(cutter.radius / pixelZLen));
    const int startPixelX = oldPosPixelX - radiusInPixelsX;
    const int startPixelZ = oldPosPixelZ - radiusInPixelsZ;

    const float crossLen = Cross((newCutterPos.vec - oldCutterPos.vec).Normalize(), alg::Vec3(0.f, 1.f, 0.f)).LengthSquared();
    const bool straightDown = crossLen < 1e-5;

    for (int x = startPixelX; x <= startPixelX + 2*radiusInPixelsX; x++) {
        for (int z = startPixelZ; z <= startPixelZ + 2*radiusInPixelsZ; z++) {
            if (x < 0 || x >= material.XResolution() || z < 0 || z >= material.ZResolution())
                continue;

            const float globalX = material.GlobalX(x);
            const float globalZ = material.GlobalZ(z);

            const float cutterY = cutter.YCoordinate(oldCutterPos, globalX, globalZ);

            UpdateHeightMap(x, z, cutterY, cutter.height, straightDown);
        }
    }

    // Step 2: Milling along the whole line
    const alg::IVec2 oldPosPixel(oldPosPixelX, oldPosPixelZ);
    const alg::IVec2 newPosPixel(newPosPixelX, newPosPixelZ);

    const auto lineDrawer = GetLineDrawer(oldPosPixel, newPosPixel);

    const LineSegment lineSegment(oldCutterPos.vec, newCutterPos.vec);
    const bool lineIsSteep = IsSteepLine(oldPosPixel, newPosPixel);

    const int cutterRadius = lineIsSteep ? radiusInPixelsZ : radiusInPixelsX;

    do {
        for (int i = -cutterRadius; i <= cutterRadius; i++) {
            const alg::IVec2& actPoint = lineDrawer->ActualPoint();

            const int x = lineIsSteep ? actPoint.X() + i : actPoint.X();
            const int z = !lineIsSteep ? actPoint.Y() + i : actPoint.Y();

            if (x < 0 || x >= material.XResolution() || z < 0 || z >= material.ZResolution())
                continue;

            const float globalX = material.GlobalX(x);
            const float globalZ = material.GlobalZ(z);

            const auto cutterPos = lineSegment.NearestPoint(alg::Vec3(globalX, 0.f, globalZ));
            const float cutterY = cutter.YCoordinate(cutterPos, globalX, globalZ);

            UpdateHeightMap(x, z, cutterY, cutter.height, straightDown);
        }
    } while (lineDrawer->NextPoint());
}


float MillingMachineSystem::UpdateHeightMap(const int x, const int z, const float cutterY, const float cutterHeight, const bool pathToDown)
{
    const float oldHeight = material.HeightAt(x, z);
    const float diff = std::max(oldHeight - cutterY, 0.f);
    if (diff == 0.f)
        return diff;

    const float newHeight = oldHeight - diff;
    material.ChangeHeightAt(x, z, newHeight);

    if (newHeight < material.BaseLevel()) {
        auto const& path = coordinator->GetComponent<MillingMachinePath>(*entities.begin());
        const auto commandId = path.commands[actCommand].id;
        millingWarnings.AddWarning(commandId, MillingWarningsRepo::MillingUnderTheBase);
    }

    if (diff > cutterHeight) {
        auto const& path = coordinator->GetComponent<MillingMachinePath>(*entities.begin());
        const auto commandId = path.commands[actCommand].id;
        millingWarnings.AddWarning(commandId, MillingWarningsRepo::MillingTooDeep);
    }

    if (pathToDown) {
        auto const& path = coordinator->GetComponent<MillingMachinePath>(*entities.begin());
        const auto commandId = path.commands[actCommand].id;
        millingWarnings.AddWarning(commandId, MillingWarningsRepo::MillingStraightDown);
    }

    return diff;
}


void MillingMachineSystem::RenderPaths(const alg::Mat4x4 &cameraMtx) const
{
    const auto& shaderRepo = ShaderRepository::GetInstance();
    auto const& shader = shaderRepo.GetStdShader();

    shader.Use();

    // Neon green
    shader.SetColor(alg::Vec4(23.f/255.f, 227.f/255.f, 33.f/255.f, 1.f));
    shader.SetMVP(cameraMtx);

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);
    }
}


void MillingMachineSystem::RenderCutter(const alg::Mat4x4 &cameraMtx) const
{
    const auto& shaderRepo = ShaderRepository::GetInstance();
    auto const& shader = shaderRepo.GetStdShader();

    auto const& pos = coordinator->GetComponent<Position>(millingCutter);
    auto const& scale = coordinator->GetComponent<Scale>(millingCutter);

    shader.SetMVP(cameraMtx * pos.TranslationMatrix() * scale.ScaleMatrix());
    shader.SetColor(alg::Vec4(1.f));
    auto const& cutterMesh = coordinator->GetComponent<Mesh>(millingCutter);
    cutterMesh.Use();

    glDrawElements(GL_TRIANGLES, cutterMesh.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);
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
