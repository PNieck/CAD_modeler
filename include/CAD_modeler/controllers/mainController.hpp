#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include "glController.hpp"
#include "guiController.hpp"
#include "../model.hpp"


class MainController {
public:
    MainController(int window_width, int window_height);

    
    void Render();

    void SizeChanged(int width, int height) {};
    inline void MouseClicked(MouseButton button) { glController.MouseClick(button); }
    inline void MouseReleased(MouseButton button) { glController.MouseRelease(button); }
    inline void MouseMoved(int x, int y) { glController.MouseMove(x, y); }

private:
    Model model;

    GlController glController;
    GuiController GuiController;
};


#endif
