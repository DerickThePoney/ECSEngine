#include "stdafx.h"

#include "InputCommands.h"

namespace ECSEngine
{
namespace EInputType
{
const std::string ToString(const Type parInputType)
{
    switch (parInputType)
    {
    case PRESSED:
        return "Pressed";
    case RELEASED:
        return "Released";
    case REPEATED:
        return "Repeated";
    default:
        AssertNotReached();
        break;
    }
    return "Invalid";
}
} // namespace EInputType

namespace
{
bool EvaluateButton(const InputKeyNames::Type parKey, const EInputType::Type parType)
{
    const bool buttonIsDown = Input::GetButtonDown(parKey);
    switch (parType)
    {
    case EInputType::PRESSED:
        return Input::GetButtonHasChanged(parKey) && buttonIsDown;
    case EInputType::RELEASED:
        return Input::GetButtonHasChanged(parKey) && !buttonIsDown;
    case EInputType::REPEATED:
        return buttonIsDown;
    default:
        AssertNotReached();
        return false;
        break;
    }
}
} // namespace

bool KeyboardCommand::Evaluate() const
{
    const bool buttonState = EvaluateButton(FKeyboardKey, FInputType);
    const bool shift = Input::IsShiftDown();
    const bool ctrl = Input::IsCtrlDown();
    const bool alt = Input::IsAltDown();

    return buttonState && (ctrl == FControl) && (shift == FShift) && (alt == FAlt);
}

namespace
{
bool EvaluateMouseButton(const MouseButtons::Type parButton, const EInputType::Type parType)
{
    const bool buttonIsDown = Input::GetMouseButtonState(parButton);
    switch (parType)
    {
    case EInputType::PRESSED:
        return Input::GetMouseButtonHasChanged(parButton) && buttonIsDown;
    case EInputType::RELEASED:
        return Input::GetMouseButtonHasChanged(parButton) && !buttonIsDown;
    case EInputType::REPEATED:
        return buttonIsDown;
    default:
        AssertNotReached();
        return false;
        break;
    }
}
} // namespace

bool MouseButtonCommand::Evaluate() const
{
    const bool buttonState = EvaluateMouseButton(FMouseButton, FInputType);
    const bool shift = Input::IsShiftDown();
    const bool ctrl = Input::IsCtrlDown();
    const bool alt = Input::IsAltDown();

    return buttonState && (ctrl == FControl) && (shift == FShift) && (alt == FAlt);
}

bool MouseScrollCommand::Evaluate() const
{
    const glm::vec2 scrollDelta = Input::GetMouseScrollDelta();
    return std::max(std::abs(scrollDelta.x), std::abs(scrollDelta.y)) > 0.f;
}

bool MouseMoveCommand::Evaluate() const
{
    return glm::length2(Input::GetMousePositionDelta()) > 0.f;
}

} // namespace ECSEngine
