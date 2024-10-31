#pragma once

#include "model.hpp"

#include "systems/gridSystem.hpp"
#include "systems/millingMachineSystem.hpp"


class MillingMachineSim final: public Model {
public:
    MillingMachineSim(int viewportWidth, int viewportHeight);

    void Update(double dt) const;

    void AddMillingPath(const std::string& filePath) const;

    [[nodiscard]]
    int GetMaterialXResolution() const
        { return millingMachineSystem->GetMaterialXResolution(); }

    [[nodiscard]]
    int GetMaterialZResolution() const
        { return millingMachineSystem->GetMaterialZResolution(); }

    void SetMaterialResolution(const int xRes, const int zRes) const
        { millingMachineSystem->SetMaterialResolution(xRes, zRes); }

    float GetMaterialXLength() const
        { return millingMachineSystem->GetMaterialXLength(); }

    float GetMaterialZLength() const
        { return millingMachineSystem->GetMaterialZLength(); }

    void SetMaterialLength(float xLen, float zLen)
        { millingMachineSystem->SetMaterialSize(xLen, zLen); }

    float GetMaterialThickness()
        { return millingMachineSystem->GetInitMaterialThickness(); }

    void SetMaterialThickness(float thickness)
        { millingMachineSystem->SetInitMaterialThickness(thickness); }

    float GetMaterialBaseLevel() const
        { return millingMachineSystem->GetBaseLevel(); }

    void SetMaterialBaseLevel(const float level)
        { millingMachineSystem->SetBaseLevel(level); }

    void ResetSimulation() const
        { millingMachineSystem->ResetSimulation(); }

    void StartSimulation() const
        { millingMachineSystem->StartMachine(); }

    void StopSimulation() const
        { millingMachineSystem->StopMachine(); }

    float GetCutterSpeed() const
        { return millingMachineSystem->GetCuterSpeed(); }

    void SetCutterSpeed(float speed)
        { millingMachineSystem->SetCutterSpeed(speed); }

    void SetCutterHeight(float height)
        { millingMachineSystem->SetCutterHeight(height); }

    [[nodiscard]]
    std::optional<MillingCutter> GetMillingCutter() const
        { return millingMachineSystem->GetMillingCutter(); }

    [[nodiscard]]
    bool MillingMachineRuns() const
        { return millingMachineSystem->MachineRuns(); }

    const auto& GetMillingWarnings() const
        { return millingMachineSystem->GetWarnings(); }

private:
    void RenderSystemsObjects(const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, float nearPlane,
        float farPlane) const override;

    std::shared_ptr<GridSystem> gridSystem;
    std::shared_ptr<MillingMachineSystem> millingMachineSystem;
};
