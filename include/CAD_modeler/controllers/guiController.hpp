#ifndef GUI_CONTROLLER_H
#define GUI_CONTROLLER_H

#include "../model.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


class GuiController {
public:
    GuiController(Model& model);

    bool WantCaptureMouse(int xPos, int yPos);

    void TorusBigRadiusChanged(float newRadius);

    void TorusSmallRadiusChanged(float newRadius);

private:
    Model& model;
};


#endif