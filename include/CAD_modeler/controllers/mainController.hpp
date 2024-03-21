#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include "glController.hpp"
#include "guiController.hpp"
#include "../model.hpp"


class MainController {
public:
    MainController(int window_width, int window_height);

    void SizeChanged(int width, int height) {};
    void Render();

private:
    GlController glController;
    GuiController GuiController;

    Model model;
};


#endif
