#pragma once

#include "utils/mouseState.hpp"
#include "../model/model.hpp"
#include "utils/keyboardState.hpp"


class SubController {
public:
    explicit SubController(Model& model): model(model) {}

    virtual ~SubController() = default;

    virtual void WindowSizeChanged(const int width, const int height)
        { model.ChangeViewportSize(width, height); }

    virtual void MouseRelease(const MouseButton button)
        { mouseState.ButtonReleased(button); }

    virtual void MouseClick(const MouseButton button)
        { mouseState.ButtonClicked(button); }

    virtual void MouseMove(int x, int y);

    virtual void ScrollMoved(int offset);

    virtual void KeyboardKeyPressed(const KeyboardKey key)
        { keyState.KeyPressed(key); }

    virtual void KeyboardKeyReleased(const KeyboardKey key)
        { keyState.KeyReleased(key); }

    virtual void Render() = 0;
    virtual void Update(double dt) = 0;

protected:
    static constexpr float ROTATION_COEF = 0.02f;
    static constexpr float TRANSLATION_COEF = 0.01f;
    static constexpr float SCROLL_COEF = 0.7f;

    static MouseState mouseState;
    static KeyboardState keyState;

private:
    Model& model;
};
