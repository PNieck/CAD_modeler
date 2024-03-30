#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include "glController.hpp"
#include "guiController.hpp"
#include "../views/guiView.hpp"
#include "../model.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


class MainController {
public:
    MainController(GLFWwindow* window, int window_width, int window_height);
    
    void Render();

    inline void SizeChanged(int width, int height) { glController.WindowSizeChanged(width, height); }
    inline void MouseClicked(MouseButton button) { glController.MouseClick(button); }
    inline void MouseReleased(MouseButton button) { glController.MouseRelease(button); }
    void MouseMoved(int x, int y);
    inline void ScrollMoved(int offset) { glController.ScrollMoved(offset); }


private:
    Model model;

    GlController glController;
    GuiController guiController;

    GuiView guiView;
};


#endif
