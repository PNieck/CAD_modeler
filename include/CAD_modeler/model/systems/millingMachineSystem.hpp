#pragma once

#include <ecs/system.hpp>

#include <algebra/mat4x4.hpp>

#include "../components/mesh.hpp"
#include "../components/MillingMachinePath.hpp"
#include "../components/millingCutter.hpp"


class MillingMachineSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void CreateMaterial(int xResolution, int zResolution);
    void AddPaths(MillingMachinePath&& paths, const MillingCutter& cutter);

    void StartMachine()
        { cutterRuns = true; }

    void StopMachine()
        { cutterRuns = false; }

    void Update(double dt);

    void Render(const alg::Mat4x4& view, const alg::Mat4x4& perspective, const alg::Vec3& camPos);

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

    void MillSection(const Position& oldCutterPos, const Position& newCutterPos);

    static float CutterY(const MillingCutter& cutter, const Position& cutterPos, float x, float z);

    std::vector<float> GenerateVertices();
    std::vector<uint32_t> GenerateIndices();

    std::vector<float> GeneratePathsVertices(const MillingMachinePath& paths);
    std::vector<uint32_t> GeneratePathsIndices(const MillingMachinePath& paths);
};
