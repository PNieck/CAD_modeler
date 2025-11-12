#include <CAD_modeler/model/model.hpp>

#include "glad/glad.h"


Model::Model(const int viewportWidth, const int viewportHeight):
    cameraManager(coordinator)
{
    // TODO: create class for OpenGl options
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(std::numeric_limits<uint32_t>::max());


    glEnable(GL_DEPTH_TEST);

    cameraManager.Init(viewportWidth, viewportHeight);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}


void Model::RenderFrame()
{
    switch (cameraManager.GetCurrentCameraType())
    {
        case CameraManager::CameraType::Perspective:
            RenderPerspectiveFrame();
            break;

        case CameraManager::CameraType::Anaglyphs:
            RenderAnaglyphsFrame();
            break;

        default:
            throw std::runtime_error("Unknown camera type");
    }
}


void Model::ChangeViewportSize(const int width, const int height)
{
    glViewport(0, 0, width, height);

    auto params = cameraManager.GetBaseParams();
    params.viewportWidth = width;
    params.viewportHeight = height;

    cameraManager.SetBaseParams(params);
}


std::tuple<int, int> Model::GetViewportSize() const
{
    GLint viewportSize[4];
    glGetIntegerv(GL_VIEWPORT, viewportSize);

    return std::make_tuple(viewportSize[2], viewportSize[3]);
}


void Model::RenderAnaglyphsFrame()
{
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render picture for the left eye
    cameraManager.SetCurrentEye(CameraManager::Eye::Left);
    alg::Mat4x4 persMtx = cameraManager.PerspectiveMtx();
    alg::Mat4x4 viewMtx = cameraManager.ViewMtx();
    const auto camParams = cameraManager.GetBaseParams();

    glColorMask(true, false, false, false);
    RenderSystemsObjects(viewMtx, persMtx, camParams.nearPlane, camParams.farPlane);

    // Render picture for right eye
    cameraManager.SetCurrentEye(CameraManager::Eye::Right);
    persMtx = cameraManager.PerspectiveMtx();
    viewMtx = cameraManager.ViewMtx();

    glColorMask(false, true, true, false);
    RenderSystemsObjects(viewMtx, persMtx, camParams.nearPlane, camParams.farPlane);

    // Setting mask back to normal
    glColorMask(true, true, true, true);
}


void Model::RenderPerspectiveFrame() const
{
    const alg::Mat4x4 persMtx = cameraManager.PerspectiveMtx();
    const alg::Mat4x4 viewMtx = cameraManager.ViewMtx();
    const auto camParams = cameraManager.GetBaseParams();

    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderSystemsObjects(viewMtx, persMtx, camParams.nearPlane, camParams.farPlane);
}
