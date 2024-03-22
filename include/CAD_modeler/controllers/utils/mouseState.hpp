#ifndef MOUSE_STATE_H
#define MOUSE_STATE_H

#include <vector>
#include <glm/vec2.hpp>


enum MouseButton {
    Right = 0,
    Middle = 1,
    Left = 2
};


class MouseState {
public:
    MouseState();

    inline void ButtonClicked(MouseButton button) { buttonClicked[button] = true; }
    inline void ButtonReleased(MouseButton button) { buttonClicked[button] = false; }
    
    inline bool IsButtonClicked(MouseButton button) const { return buttonClicked[button]; }

    void Moved(int x, int y);

    inline glm::ivec2 TranslationGet() const { return actMousePos - oldMousePos; }

private:
    static const int BUTTON_CNT = 3;

    bool buttonClicked[BUTTON_CNT];

    glm::ivec2 oldMousePos;
    glm::ivec2 actMousePos;
};


#endif