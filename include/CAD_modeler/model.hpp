#pragma once

#include "model/ecsCoordinator.hpp"
#include "model/systems/cameraSystem.hpp"
#include "model/systems/meshRenderer.hpp"
#include "model/systems/toriSystem.hpp"
#include "model/systems/gridSystem.hpp"
#include "model/systems/cursorSystem.hpp"
#include "model/systems/pointsSystem.hpp"


class Model
{
public:
    Model(int viewport_width, int viewport_height);

    void RenderFrame();

    void AddTorus();

    inline void RotateCamera(float x, float y) const
        { cameraSys->RotateAroundTarget(x, y); }

    inline float GetCameraDistanceFromTarget() const
        { return cameraSys->GetDistanceToTarget(); }

    inline void SetCameraDistance(float newDist) const
        { cameraSys->SetDistanceToTarget(newDist); }

    inline void MultiplyCameraDistanceFromTarget(float coefficient) const
        { cameraSys->MultiplyDistanceToTarget(coefficient); }

    void ChangeViewportSize(int width, int height);

    inline void SetCursorPosition(float x, float y, float z) const
        { cursorSystem->SetPosition(glm::vec3(x, y, z)); }

    // TODO; Change to normal coordinates
    void SetCursorPositionFromViewport(float x, float y) const;

    void Add3DPointFromViewport(float x, float y) const;

private:
    Coordinator coordinator;

    std::shared_ptr<CameraSystem> cameraSys;
    std::shared_ptr<MeshRenderer> meshRenderer;
    std::shared_ptr<ToriSystem> toriSystem;
    std::shared_ptr<GridSystem> gridSystem;
    std::shared_ptr<CursorSystem> cursorSystem;
    std::shared_ptr<PointsSystem> pointsSystem;

    glm::vec3 PointFromViewportCoordinates(float x, float y) const;
};
