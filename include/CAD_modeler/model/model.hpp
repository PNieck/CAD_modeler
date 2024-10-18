#pragma once

#include <ecs/coordinator.hpp>

#include "managers/cameraManager.hpp"


class Model {
public:
    virtual ~Model() = default;

    Model(int viewportWidth, int viewportHeight);

    void RenderFrame();

    void ChangeViewportSize(int width, int height);

    [[nodiscard]]
    std::tuple<int, int> GetViewportSize() const;

    CameraManager cameraManager;
protected:
    Coordinator coordinator;

    virtual void RenderSystemsObjects(
        const alg::Mat4x4& viewMtx, const alg::Mat4x4& persMtx, float nearPlane, float farPlane) const = 0;

private:
    void RenderAnaglyphsFrame();

    void RenderPerspectiveFrame() const;
};
