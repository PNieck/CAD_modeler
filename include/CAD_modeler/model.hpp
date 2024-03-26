#ifndef MODEL_H
#define MODEL_H

#include "model/ecsCoordinator.hpp"
#include "model/systems/cameraSystem.hpp"
#include "model/systems/meshRenderer.hpp"
#include "model/systems/toriSystem.hpp"


class Model
{
public:
    Model(int viewport_width, int viewport_height);

    void RenderFrame();

    void AddTorus();

private:
    Coordinator coordinator;

    std::shared_ptr<CameraSystem> cameraSys;
    std::shared_ptr<MeshRenderer> meshRenderer;
    std::shared_ptr<ToriSystem> toriSystem;
};


#endif