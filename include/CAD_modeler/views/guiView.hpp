#ifndef GUI_VIEW_H
#define GUI_VIEW_H

#include "../controllers/guiController.hpp"
#include "../model.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


class GuiView {
public:
    GuiView(GLFWwindow* window);
    ~GuiView();

    void RenderGui(GuiController& controller, const Model& model) const;
};


#endif
