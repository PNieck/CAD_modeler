#pragma once

#include "model.hpp"

#include "systems/gridSystem.hpp"


class MillingMachineSim final: public Model {
public:
    MillingMachineSim(int viewportWidth, int viewportHeight);

private:
    void RenderSystemsObjects(const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, float nearPlane,
        float farPlane) const override;

    std::shared_ptr<GridSystem> gridSystem;
};
