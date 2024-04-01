#include <CAD_modeler/model.hpp>

#include <CAD_modeler/utilities/line.hpp>
#include <CAD_modeler/utilities/plane.hpp>


Model::Model(int viewport_width, int viewport_height)
{
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(std::numeric_limits<uint32_t>::max());

    CameraSystem::RegisterSystem(coordinator);
    MeshRenderer::RegisterSystem(coordinator);
    ToriSystem::RegisterSystem(coordinator);
    GridSystem::RegisterSystem(coordinator);
    CursorSystem::RegisterSystem(coordinator);

    cameraSys = coordinator.GetSystem<CameraSystem>();
    meshRenderer = coordinator.GetSystem<MeshRenderer>();
    toriSystem = coordinator.GetSystem<ToriSystem>();
    gridSystem = coordinator.GetSystem<GridSystem>();
    cursorSystem = coordinator.GetSystem<CursorSystem>();

    cameraSys->Init(viewport_width, viewport_height);
    gridSystem->Init();
    cursorSystem->Init();
    

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}


void Model::RenderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);

    gridSystem->Render();
    meshRenderer->Render();
    cursorSystem->Render();
}


void Model::AddTorus()
{
    toriSystem->AddTorus();
}


void Model::ChangeViewportSize(int width, int height)
{
    glViewport(0, 0, width, height);
    cameraSys->ChangeViewportSize(width, height);
}

void Model::SetCursorPositionFromWindowPoint(float x, float y) const
{
    auto viewMtx = cameraSys->ViewMatrix();
    auto perspectiveMtx = cameraSys->PerspectiveMatrix();
    auto const& cameraTarget = cameraSys->GetTargetPosition();
    auto const& cameraPos = cameraSys->GetPosition();
    auto cursorPos = cursorSystem->GetPosition();

    auto cameraInv = glm::inverse(perspectiveMtx * viewMtx);

    glm::vec4 near_v4 = cameraInv * glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 far_v4 = cameraInv * glm::vec4(x, y, 1.0f, 1.0f);

    glm::vec3 near = glm::vec3(near_v4.x / near_v4.w, near_v4.y / near_v4.w, near_v4.z / near_v4.w);
    glm::vec3 far = glm::vec3(far_v4.x / far_v4.w, far_v4.y / far_v4.w, far_v4.z / far_v4.w);

    Line nearToFar = Line::FromTwoPoints(near, far);

    glm::vec3 cameraDirection = cameraTarget.vec - cameraPos.vec;
    Plane perpendicularToScreenWithCursor(cursorPos.vec, cameraDirection);

    std::optional<glm::vec3> intersection =
        perpendicularToScreenWithCursor.Intersect(nearToFar);
    
    if (!intersection.has_value()) {
        return;
    }

    cursorSystem->SetPosition(intersection.value());
}
