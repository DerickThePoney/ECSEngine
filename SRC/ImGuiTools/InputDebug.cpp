#include "stdafx.h"

#include "InputDebug.h"

#include "Common/InputManager.h"
#include "RenderingCore/GLFWWrapper.h"

namespace ECSEngine
{
namespace ImGUITools
{
void InputDebug(bool* shouldBeOpen)
{
    ImGui::Begin("Input Debug", shouldBeOpen);
    vec2 mousePos = Input::GetMousePosition();
    vec2 mousePosDelta = Input::GetMousePositionDelta();
    vec2 mouseScroll = Input::GetMouseScrollDelta();

    ImGui::InputFloat2("Mouse position", (float*)&mousePos, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat2("Mouse delta", (float*)&mousePosDelta, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat2("Mouse scroll", (float*)&mouseScroll, "%.3f", ImGuiInputTextFlags_ReadOnly);

    forrange(i, 0, 7)
    {
        bool mouseButtonState = Input::GetMouseButtonState((int)i);
        ImGui::Checkbox("Mouse button", &mouseButtonState);
    }

    ImGui::Separator();

    bool keyboardButtonState = Input::IsShiftDown();
    ImGui::Checkbox("Shift", &keyboardButtonState);
    keyboardButtonState = Input::IsAltDown();
    ImGui::Checkbox("Alt", &keyboardButtonState);
    keyboardButtonState = Input::IsCtrlDown();
    ImGui::Checkbox("Ctrl", &keyboardButtonState);

    forrange(i, InputKeyNames::INPUT_KEY_COMMA, InputKeyNames::INPUT_KEY_RIGHT_BRACKET)
    {
        keyboardButtonState = Input::GetButtonDown((InputKeyNames::Type)i);
        ImGui::Checkbox(GLFWWrapper::GetKeyName((InputKeyNames::Type)i), &keyboardButtonState);

        if ((i - InputKeyNames::INPUT_KEY_A) % 5 != 0 || i == InputKeyNames::INPUT_KEY_A)
        {
            ImGui::SameLine();
        }
    }

    ImGui::Separator();
    forrange(i, 0, 16)
    {
        if (Input::IsGamepadConnected((int)i))
        {
            ImGui::Text(Input::GetGamepadName((int)i));

            forrange(j, 0, GamepadButtons::GAMEPAD_BUTTON_LAST)
            {
                bool isPressed = Input::GetGamepadButtonDown((int)i, (GamepadButtons::Type)j);
                ImGui::Checkbox(GamepadButtons::ToString((GamepadButtons::Type)j), &isPressed);
            }

            forrange(j, 0, GamepadAxes::GAMEPAD_AXIS_LAST)
            {
                float value = Input::GetGamepadAxisValue((int)i, (GamepadAxes::Type)j);
                ImGui::InputFloat(GamepadAxes::ToString((GamepadAxes::Type)j), &value, ImGuiInputTextFlags_ReadOnly);
            }
        }
    }
    ImGui::End();
}
} // namespace ImGUITools
} // namespace ECSEngine
