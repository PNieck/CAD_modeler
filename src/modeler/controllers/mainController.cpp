#include <CAD_modeler/controllers/mainController.hpp>


MainController::MainController(GLFWwindow* window, const int windowWidth, const int windowHeight):
    modeler(windowWidth, windowHeight),
    millingSim(windowWidth, windowHeight),
    pathsDesigner(windowWidth, windowHeight),
    millSimController(millingSim),
    modelerController(modeler),
    millingPathsDesignerController(pathsDesigner),
    guiController(window),
    actModelType(ModelType::Modeler),
    actController(ActController()),
    modelChooser(*this)
{
}


void MainController::MouseMoved(const int x, const int y)
{
    if (guiController.WantCaptureMouse())
        return;

    actController->MouseMove(x, y);
}


void MainController::MouseClicked(const MouseButton button)
{
    if (guiController.WantCaptureMouse())
        return;

    actController->MouseClick(button);
}


void MainController::MouseReleased(const MouseButton button)
{
    if (guiController.WantCaptureMouse())
        return;

    actController->MouseRelease(button);
}


void MainController::Render()
{
    guiController.PreFrameRender();

    modelChooser.Render();
    actController->Render();
    guiController.PostFrameRender();
}


void MainController::ScrollMoved(const int offset)
{
    if (guiController.WantCaptureMouse())
        return;

    actController->ScrollMoved(offset);
}


void MainController::KeyboardKeyPressed(const KeyboardKey key)
{
    if (guiController.WantCaptureKeyboard())
        return;

    actController->KeyboardKeyPressed(key);
}


void MainController::KeyboardKeyReleased(const KeyboardKey key)
{
    if (guiController.WantCaptureKeyboard())
        return;

    actController->KeyboardKeyReleased(key);
}


void MainController::SetModelType(const ModelType modelType)
{
    actModelType = modelType;
    actController = ActController();
}


SubController* MainController::ActController()
{
    switch (actModelType) {
        case ModelType::Modeler:
            return &modelerController;

        case ModelType::MillerMachineSim:
            return &millSimController;

        case ModelType::MillingPathsDesigner:
            return &millingPathsDesignerController;

        default:
            throw std::runtime_error("Unknown model type");
    }
}
