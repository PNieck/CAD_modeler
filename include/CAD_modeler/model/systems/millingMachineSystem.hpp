#pragma once

#include <ecs/system.hpp>

#include <algebra/mat4x4.hpp>

#include <optional>

#include "../components/mesh.hpp"
#include "../components/MillingMachinePath.hpp"
#include "../components/millingCutter.hpp"
#include "../components/millingWarningsRepo.hpp"


class MillingMachineSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(int xResolution, int zResolution);
    void AddPaths(MillingMachinePath&& paths, const MillingCutter& cutter);

    void StartMachine()
        { cutterRuns = true; }

    [[nodiscard]]
    bool MachineRuns() const
        { return cutterRuns; }

    void StopMachine()
        { cutterRuns = false; }

    [[nodiscard]]
    int GetMaterialXResolution() const
        { return heightMapXResolution; }

    [[nodiscard]]
    int GetMaterialZResolution() const
        { return heightMapZResolution; }

    void SetMaterialResolution(int xRes, int zRes);

    [[nodiscard]]
    float GetMaterialXLength() const
        { return heightMapXLen; }

    [[nodiscard]]
    float GetMaterialZLength() const
        { return heightMapZLen; }

    void SetMaterialSize(float xLen, float zLen);

    float GetInitMaterialThckness()
        { return initMaterialThickness; }

    void SetInitMaterialThickness(float thickness);

    float GetBaseLevel()
        { return mainHeightMapCorner.Y(); }

    void SetBaseLevel(float level);

    void SetCutterSpeed(const float newSpeed)
        { cutterSpeed = newSpeed; }

    float GetCuterSpeed() const
        { return cutterSpeed; }

    void ResetMaterial();

    std::optional<MillingCutter> GetMiliingCutter() const;

    void Update(double dt);

    void Render(const alg::Mat4x4& view, const alg::Mat4x4& perspective, const alg::Vec3& camPos);

    [[nodiscard]]
    const auto& GetWarnings() const
        { return millingWarnings; }

private:
    unsigned int heightmap = 0;

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
    int heightMapXResolution = 0;
    int heightMapZResolution = 0;
    float initMaterialThickness = 1.f;

    MillingWarningsRepo millingWarnings;

    void MillSection(const Position& oldCutterPos, const Position& newCutterPos);

    static float CutterY(const MillingCutter& cutter, const Position& cutterPos, float x, float z);

    float UpdateHeightMap(int row, int col, float cutterY, float cutterHeight, bool pathToDown);

    std::vector<float> GenerateVertices();
    std::vector<uint32_t> GenerateIndices();

    std::vector<float> GeneratePathsVertices(const MillingMachinePath& paths);
    std::vector<uint32_t> GeneratePathsIndices(const MillingMachinePath& paths);
};
