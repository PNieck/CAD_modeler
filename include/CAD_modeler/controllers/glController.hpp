#ifndef GL_CONTROLLER_H
#define GL_CONTROLLER_H

#include "utils/mouseState.hpp"
#include "../model.hpp"


class GlController {
public:
    GlController(Model& model);

    inline void MouseClick(MouseButton button) { mouseState.ButtonClicked(button); }
    inline void MouseRelease(MouseButton button) { mouseState.ButtonReleased(button); }

    void MouseMove(int x, int y);

private:
    static constexpr float ROTATION_COEFF = 0.02;

    Model& model;

    MouseState mouseState;
};


#endif