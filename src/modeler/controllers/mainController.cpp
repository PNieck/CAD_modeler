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

    actController.get().MouseMove(x, y);
}


void MainController::MouseClicked(const MouseButton button)
{
    if (guiController.WantCaptureMouse())
        return;

    actController.get().MouseClick(button);
}


void MainController::MouseReleased(const MouseButton button)
{
    if (guiController.WantCaptureMouse())
        return;

    actController.get().MouseRelease(button);
}


void MainController::Render()
{
    guiController.PreFrameRender();

    modelChooser.Render();
    actController.get().Render();
    guiController.PostFrameRender();
}


void MainController::ScrollMoved(const int offset)
{
    if (guiController.WantCaptureMouse())
        return;

    actController.get().ScrollMoved(offset);
}


void MainController::KeyboardKeyPressed(const KeyboardKey key)
{
    if (guiController.WantCaptureKeyboard())
        return;

    actController.get().KeyboardKeyPressed(key);
}


void MainController::KeyboardKeyReleased(const KeyboardKey key)
{
    if (guiController.WantCaptureKeyboard())
        return;

    actController.get().KeyboardKeyReleased(key);
}


void MainController::SetModelType(const ModelType modelType)
{
    actModelType = modelType;
    actController = ActController();
}


SubController& MainController::ActController()
{
    switch (actModelType) {
        case ModelType::Modeler:
            return modelerController;

        case ModelType::MillerMachineSim:
            return millSimController;

        case ModelType::MillingPathsDesigner:
            return millingPathsDesignerController;

        default:
            throw std::runtime_error("Unknown model type");
    }
}
