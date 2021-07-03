#include "stdafx.h"

#include "InputEvent.h"

namespace ECSEngine
{

InputEvent::InputEvent(bool parMouse, bool parMouseButton, bool parKeyboard, bool parGamepad)
    : FMouse(parMouse)
    , FMouseButton(parMouseButton)
    , FKeyboard(parKeyboard)
    , FGamepad(parGamepad)
{
}

bool InputEvent::IsMouseEvent() const
{
    return FMouse;
}

bool InputEvent::IsMouseButtonEvent() const
{
    return FMouseButton;
}

bool InputEvent::IsKeyboardEvent() const
{
    return FKeyboard;
}

bool InputEvent::IsGamepadEvent() const
{
    return FGamepad;
}

} // namespace ECSEngine
