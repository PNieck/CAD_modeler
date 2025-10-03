#pragma once

#include <ecs/system.hpp>

#include <algebra/mat4x4.hpp>

#include <optional>
#include <vector>
#include <thread>

#include "../components/mesh.hpp"
#include "../components/MillingMachinePath.hpp"
#include "../components/millingCutter.hpp"
#include "../components/millingWarningsRepo.hpp"
#include "../components/texture2D.hpp"
#include "../../utilities/asyncWorker.hpp"


class MillingMachineSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    MillingMachineSystem();

    void Init(int xResolution, int zResolution);
    void AddPaths(MillingMachinePath&& paths, const MillingCutter& cutter) const;

    void StartMachine()
        { cutterRuns = true; }

    [[nodiscard]]
    bool MachineRuns() const
        { return cutterRuns; }

    void StopMachine()
        { cutterRuns = false; }

    void StartInstantMilling();

    void StopInstantMilling();

    bool InstantMillingRuns() const
        { return instantWorker.IsWorking(); }

    [[nodiscard]]
    int GetMaterialXResolution() const
        { return heightmap.GetWidth(); }

    [[nodiscard]]
    int GetMaterialZResolution() const
        { return heightmap.GetHeight(); }

    void SetMaterialResolution(int xRes, int zRes);

    [[nodiscard]]
    float GetMaterialXLength() const
        { return heightMapXLen; }

    [[nodiscard]]
    float GetMaterialZLength() const
        { return heightMapZLen; }

    void SetMaterialSize(float xLen, float zLen);

    [[nodiscard]]
    float GetInitMaterialThickness() const
        { return initMaterialThickness; }

    void SetInitMaterialThickness(float thickness);

    float GetBaseLevel()
        { return mainHeightMapCorner.Y(); }

    void SetBaseLevel(float level);

    void SetCutterSpeed(const float newSpeed)
        { cutterSpeed = newSpeed; }

    [[nodiscard]]
    float GetCuterSpeed() const
        { return cutterSpeed; }

    void ResetMaterial();

    void ResetSimulation();

    [[nodiscard]]
    std::optional<MillingCutter> GetMillingCutter() const;

    void SetCutterHeight(float height) const;

    void Update(double dt);

    void Render(const alg::Mat4x4& view, const alg::Mat4x4& perspective, const alg::Vec3& camPos);

    [[nodiscard]]
    const auto& GetWarnings() const
        { return millingWarnings; }

private:
    Texture2D heightmap;

    Mesh material;
    Entity millingCutter = 0;
    float t = 0.f;
    bool cutterRuns = false;
    float cutterSpeed = 15.f/60.f;
    int actCommand = 1;
    std::vector<float> textureData;
    alg::Vec3 mainHeightMapCorner = alg::Vec3(-0.75f, 0.f, -0.75f);
    float heightMapZLen = 1.5f;
    float heightMapXLen = 1.5f;
    float initMaterialThickness = 0.5f;

    AsyncWorker instantWorker;

    MillingWarningsRepo millingWarnings;

    void InstantMillingThreadFunc(std::stop_token stoken);

    void MillSection(const Position& oldCutterPos, const Position& newCutterPos);

    static float CutterY(const MillingCutter& cutter, const Position& cutterPos, float x, float z);

    float UpdateHeightMap(int row, int col, float cutterY, float cutterHeight, bool pathToDown);

    void RenderMaterial(const alg::Mat4x4& cameraMtx, const alg::Vec3& camPos) const;
    void RenderPaths(const alg::Mat4x4& cameraMtx) const;
    void RenderCutter(const alg::Mat4x4& cameraMtx) const;

    std::vector<float> GenerateMaterialVertices();
    std::vector<uint32_t> GenerateMaterialIndices();

    static std::vector<float> GeneratePathsVertices(const MillingMachinePath& paths);
    static std::vector<uint32_t> GeneratePathsIndices(const MillingMachinePath& paths);
};
