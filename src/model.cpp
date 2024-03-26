#include <CAD_modeler/model.hpp>


Model::Model(int viewport_width, int viewport_height)
{
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(std::numeric_limits<uint32_t>::max());

    CameraSystem::RegisterSystem(coordinator);
    MeshRenderer::RegisterSystem(coordinator);
    ToriSystem::RegisterSystem(coordinator);

    cameraSys = coordinator.GetSystem<CameraSystem>();
    meshRenderer = coordinator.GetSystem<MeshRenderer>();
    toriSystem = coordinator.GetSystem<ToriSystem>();

    cameraSys->Init(viewport_width, viewport_height);

    AddTorus();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}


void Model::RenderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);

    meshRenderer->Render();
}


void Model::AddTorus()
{
    toriSystem->AddTorus();
}
