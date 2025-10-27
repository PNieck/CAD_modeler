#pragma once

#include "guiController.hpp"
#include "millingSimController.hpp"
#include "modelerController.hpp"
#include "millingsPathsDesignerController.hpp"

#include "utils/modelTypes.hpp"
#include "utils/keyboardKey.hpp"

#include "../model/modeler.hpp"
#include "../model/millingMachineSim.hpp"
#include "../model/millingPathsDesigner.hpp"

#include "../views/modelChooser.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


class MainController {
public:
    MainController(GLFWwindow* window, int windowWidth, int windowHeight);
    
    void Render();
    void Update(const double dt)
        { actController->Update(dt); }

    void SizeChanged(const int width, const int height)
        { actController->WindowSizeChanged(width, height); }

    void MouseClicked(MouseButton button);
    void MouseReleased(MouseButton button);
    void MouseMoved(int x, int y);
    void ScrollMoved(int offset);
    void KeyboardKeyPressed(KeyboardKey key);
    void KeyboardKeyReleased(KeyboardKey key);

    [[nodiscard]]
    ModelType GetModelType() const
        { return actModelType; }

    void SetModelType(ModelType modelType);

private:
    Modeler modeler;
    MillingMachineSim millingSim;
    MillingPathsDesigner pathsDesigner;

    MillingSimController millSimController;
    ModelerController modelerController;
    MillingsPathsDesignerController millingPathsDesignerController;
    GuiController guiController;

    ModelType actModelType;
    SubController* actController;

    ModelChooser modelChooser;

    SubController* ActController();
};
