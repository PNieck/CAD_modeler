#include <CAD_modeler/controllers/utils/mouseState.hpp>


MouseState::MouseState():
    oldMousePos(0), actMousePos(0)
{
    for (bool& i : buttonClicked) {
        i = false;
    }
}


void MouseState::Moved(const int x, const int y)
{
    oldMousePos = actMousePos;

    actMousePos.X() = x;
    actMousePos.Y() = y;
}
