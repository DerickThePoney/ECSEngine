#pragma once

namespace ECSEngine
{
class InputEvent
{
public:
    InputEvent(bool parMouse, bool parMouseButton, bool parKeyboard, bool parGamepad);

    bool IsMouseEvent() const;
    bool IsMouseButtonEvent() const;
    bool IsKeyboardEvent() const;
    bool IsGamepadEvent() const;

private:
    bool FMouse = false;
    bool FMouseButton = false;
    bool FKeyboard = false;
    bool FGamepad = false;
};
} // namespace ECSEngine
