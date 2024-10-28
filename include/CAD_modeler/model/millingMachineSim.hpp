#pragma once

#include "model.hpp"

#include "systems/gridSystem.hpp"
#include "systems/millingMachineSystem.hpp"


class MillingMachineSim final: public Model {
public:
    MillingMachineSim(int viewportWidth, int viewportHeight);

    void Update(double dt) const;

private:
    void RenderSystemsObjects(const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, float nearPlane,
        float farPlane) const override;

    std::shared_ptr<GridSystem> gridSystem;
    std::shared_ptr<MillingMachineSystem> millingMachineSystem;
};
