#pragma once

#include <ecs/system.hpp>

#include <algebra/mat4x4.hpp>

#include <optional>
#include <vector>
#include <thread>

#include "../components/millingMaterial.hpp"
#include "../components/millingMachinePath.hpp"
#include "../components/millingCutter.hpp"
#include "../components/millingWarningsRepo.hpp"
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
        { return material.XResolution(); }

    [[nodiscard]]
    int GetMaterialZResolution() const
        { return material.ZResolution(); }

    void SetMaterialResolution(const int xRes, const int zRes)
        { material.SetResolution(xRes, zRes); }

    [[nodiscard]]
    float GetMaterialXLength() const
        { return material.XLength(); }

    [[nodiscard]]
    float GetMaterialZLength() const
        { return material.ZLength(); }

    void SetMaterialSize(const float xLen, const float zLen)
        { material.SetSize(xLen, zLen); }

    [[nodiscard]]
    float GetInitMaterialThickness() const
        { return material.InitThickness(); }

    void SetInitMaterialThickness(const float thickness)
        { material.SetThickness(thickness); }

    float GetBaseLevel() const
        { return material.BaseLevel(); }

    void SetBaseLevel(const float level)
        { material.SetBaseLevel(level); }

    void SetCutterSpeed(const float newSpeed)
        { cutterSpeed = newSpeed; }

    [[nodiscard]]
    float GetCuterSpeed() const
        { return cutterSpeed; }

    void ResetMaterial()
        { material.Reset(); }

    void ResetSimulation();

    [[nodiscard]]
    std::optional<MillingCutter> GetMillingCutter() const;

    void SetCutterHeight(float height) const;

    void Update(double dt);

    void Render(const alg::Mat4x4& view, const alg::Mat4x4& perspective, const alg::Vec3& camPos);

    [[nodiscard]]
    const auto& GetWarnings() const
        { return millingWarnings; }

    void ClearWarnings()
        { millingWarnings.Clear(); }

private:
    MillingMaterial material;

    Entity millingCutter = 0;
    float t = 0.f;
    bool cutterRuns = false;
    float cutterSpeed = 15.f/60.f;
    int actCommand = 1;

    AsyncWorker instantWorker;

    MillingWarningsRepo millingWarnings;

    void InstantMillingThreadFunc(std::stop_token stoken);

    void MillSection(const Position& oldCutterPos, const Position& newCutterPos);

    float UpdateHeightMap(int x, int z, float cutterY, float cutterHeight, bool pathToDown);

    void RenderPaths(const alg::Mat4x4& cameraMtx) const;
    void RenderCutter(const alg::Mat4x4& cameraMtx) const;

    static std::vector<float> GeneratePathsVertices(const MillingMachinePath& paths);
    static std::vector<uint32_t> GeneratePathsIndices(const MillingMachinePath& paths);
};
