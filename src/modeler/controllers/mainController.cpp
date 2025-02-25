#include <CAD_modeler/controllers/mainController.hpp>


MainController::MainController(GLFWwindow* window, const int windowWidth, const int windowHeight):
    modeler(windowWidth, windowHeight),
    millingSim(windowWidth, windowHeight),
    millSimController(millingSim),
    modelerController(modeler),
    guiController(window),
    actModelType(ModelType::Modeler),
    modelChooser(*this)
{
}


void MainController::MouseMoved(const int x, const int y)
{
    if (guiController.WantCaptureMouse())
        return;

    switch (actModelType) {
        case ModelType::Modeler:
            modelerController.MouseMove(x, y);
        break;

        case ModelType::MillerMachineSim:
            millSimController.MouseMove(x, y);
            break;

        default:
            throw std::runtime_error("Unknown model type");
    }
}


void MainController::MouseClicked(const MouseButton button)
{
    if (guiController.WantCaptureMouse())
        return;

    switch (actModelType) {
        case ModelType::Modeler:
            modelerController.MouseClick(button);
            break;

        case ModelType::MillerMachineSim:
            millSimController.MouseClick(button);
            break;

        default:
            throw std::runtime_error("Unknown model type");
    }
}


void MainController::MouseReleased(const MouseButton button)
{
    if (guiController.WantCaptureMouse())
        return;

    switch (actModelType) {
        case ModelType::Modeler:
            modelerController.MouseRelease(button);
            break;

        case ModelType::MillerMachineSim:
            millSimController.MouseRelease(button);
            break;

        default:
            throw std::runtime_error("Unknown model type");
    }
}


void MainController::Render()
{
    guiController.PreFrameRender();

    modelChooser.Render();

    switch (actModelType) {
        case ModelType::Modeler:
            modelerController.Render();
            break;

        case ModelType::MillerMachineSim:
            millSimController.Render();
            break;

        default:
            throw std::runtime_error("Unknown model type");
    }

    guiController.PostFrameRender();
}


void MainController::Update(double dt)
{
    if (actModelType == ModelType::MillerMachineSim)
        millSimController.Update(dt);
}


void MainController::SizeChanged(const int width, const int height)
{
    switch (actModelType) {
        case ModelType::Modeler:
            modelerController.WindowSizeChanged(width, height);
        break;

        case ModelType::MillerMachineSim:
            millSimController.WindowSizeChanged(width, height);
        break;

        default:
            throw std::runtime_error("Unknown model type");
    }
}


void MainController::ScrollMoved(const int offset)
{
    if (guiController.WantCaptureMouse())
        return;

    switch (actModelType) {
        case ModelType::Modeler:
            modelerController.ScrollMoved(offset);
            break;

        case ModelType::MillerMachineSim:
            millSimController.ScrollMoved(offset);
            break;

        default:
            throw std::runtime_error("Unknown model type");
    }
}


void MainController::KeyboardKeyPressed(const KeyboardKey key)
{
    switch (actModelType) {
        case ModelType::Modeler:
            modelerController.KeyboardKeyPressed(key);
        break;

        case ModelType::MillerMachineSim:
            break;

        default:
            throw std::runtime_error("Unknown model type");
    }
}
