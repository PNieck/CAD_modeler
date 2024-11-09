#pragma once

#include "guiController.hpp"
#include "millingSimController.hpp"
#include "modelerController.hpp"
#include "utils/modelTypes.hpp"
#include "utils/keyboardKey.hpp"

#include "../model/modeler.hpp"
#include "../model/millingMachineSim.hpp"

#include "../views/modelChooser.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


class MainController {
public:
    MainController(GLFWwindow* window, int windowWidth, int windowHeight);
    
    void Render();
    void Update(double dt);

    void SizeChanged(int width, int height);
    void MouseClicked(MouseButton button);
    void MouseReleased(MouseButton button);
    void MouseMoved(int x, int y);
    void ScrollMoved(int offset);
    void KeyboardKeyPressed(KeyboardKey key);

    [[nodiscard]]
    inline ModelType GetModelType() const
        { return actModelType; }

    void SetModelType(const ModelType modelType)
        { actModelType = modelType; }

private:
    Modeler modeler;
    MillingMachineSim millingSim;

    MillingSimController millSimController;
    ModelerController modelerController;
    GuiController guiController;

    ModelType actModelType;

    ModelChooser modelChooser;
};
