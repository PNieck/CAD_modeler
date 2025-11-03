#pragma once

#include <vector>


enum class KeyboardKey {
    a,
    c,
    left_shift,
    right_shift,
    left_control,
    right_control,
};


class KeyboardState {
public:
    KeyboardState(): pressed(6, false) {}

    void KeyPressed(KeyboardKey key)
        { pressed[static_cast<size_t>(key)] = true; }

    void KeyReleased(KeyboardKey key)
        { pressed[static_cast<size_t>(key)] = false; }

    [[nodiscard]]
    bool IsKeyPressed(KeyboardKey key) const
        { return pressed[static_cast<size_t>(key)]; }

    [[nodiscard]]
    bool ShiftPressed() const
        { return IsKeyPressed(KeyboardKey::left_shift) || IsKeyPressed(KeyboardKey::right_shift); }

    [[nodiscard]]
    bool ControlPressed() const
        { return IsKeyPressed(KeyboardKey::left_control) || IsKeyPressed(KeyboardKey::right_control); }

private:
    std::vector<bool> pressed;
};
