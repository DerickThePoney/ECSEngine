#include "stdafx.h"

#include "InputManager.h"

#include "Logger.h"

namespace ECSEngine
{

namespace GamepadButtons
{
#define DECLARE_ENUM(NAME, ID)                                                                                                                                                     \
    {                                                                                                                                                                              \
        (Type) ID, #NAME                                                                                                                                                           \
    }
std::map<Type, const char*> GGButtonsNames = {
#include "GamepadButtons.inl"
};
#undef DECLARE_ENUM
const char* ToString(const Type parValue)
{
    AssertRelease(GGButtonsNames.find(parValue) != GGButtonsNames.end());
    return GGButtonsNames[parValue];
}

} // namespace GamepadButtons

namespace GamepadAxes
{
#define DECLARE_ENUM(NAME, ID)                                                                                                                                                     \
    {                                                                                                                                                                              \
        (Type) ID, #NAME                                                                                                                                                           \
    }
std::map<Type, const char*> GAxesNames = {
#include "GamepadAxes.inl"
};
#undef DECLARE_ENUM
const char* ToString(const Type parValue)
{
    AssertRelease(GAxesNames.find(parValue) != GAxesNames.end());
    return GAxesNames[parValue];
}

} // namespace GamepadAxes

namespace InputKeyNames
{
#define DECLARE_ENUM(NAME, ID)                                                                                                                                                     \
    {                                                                                                                                                                              \
        (Type) ID, #NAME                                                                                                                                                           \
    }
std::map<Type, const char*> GKeyNames = {
#include "InputKeyNames.inl"
};
#undef DECLARE_ENUM
const char* ToString(const Type parValue)
{
    AssertRelease(GKeyNames.find(parValue) != GKeyNames.end());
    return GKeyNames[parValue];
}

} // namespace InputKeyNames

namespace MouseButtons
{
#define DECLARE_ENUM(NAME, ID)                                                                                                                                                     \
    {                                                                                                                                                                              \
        (Type) ID, #NAME                                                                                                                                                           \
    }
std::map<Type, const char*> GMouseButtonsNames = {
#include "MouseButtons.inl"
};
#undef DECLARE_ENUM
const char* ToString(const Type parValue)
{
    AssertRelease(GMouseButtonsNames.find(parValue) != GMouseButtonsNames.end());
    return GMouseButtonsNames[parValue];
}

} // namespace MouseButtons

class InputManager final : public Singleton<InputManager>
{
public:
    InputManager();
    ~InputManager();

    void Initialise(const u32 parNbKeyboardKeys, const vec2& parMousePosition, const u32 parNbMouseButtons);
    void EndFrame();
    void Shutdown();

    void SetMousePosition(const vec2& parMousePosition);
    void SetMouseScrollDelta(const vec2& parMouseScrollDelta);
    void SetMouseButtonState(int button, bool value);
    void SetKeyboardButtonState(int button, bool value, bool isShiftDown, bool isCtrlDown, bool isAltDown);

    void AddCharacterInput(u32 character);
    bool TextInputHasChanged() const;
    MemoryView<const u32> TextInput() const;

    void SetInputsAlreadyUsed(const bool parKeyboardInputUsed, const bool parMouseInputUsed);
    void GetInputsAlreadyUsed(bool& parKeyboardInputUsed, bool& parMouseInputUsed);

    const vec2 GetMousePosition() { return (FMouse.AlreadyUsed) ? vec2(0.f) : FMouse.MousePosition; }
    const vec2 GetMousePositionDelta() { return (FMouse.AlreadyUsed) ? vec2(0.f) : FMouse.MousePosition - FMouse.PreviousMousePosition; }
    const vec2 GetMouseScrollDelta() { return (FMouse.AlreadyUsed) ? vec2(0.f) : FMouse.MouseScrollDelta; }
    bool GetMouseButtonState(int button)
    {
        AssertRelease(button < FMouse.MouseButtons.ThisFrameValues.size());
        return !FMouse.AlreadyUsed && FMouse.MouseButtons.ThisFrameValues[button];
    }
    bool GetMouseButtonHasChanged(int button)
    {
        AssertRelease(button < FMouse.MouseButtons.ThisFrameValues.size());
        return !FMouse.AlreadyUsed && FMouse.MouseButtons.ThisFrameValues[button] != FMouse.MouseButtons.PreviousFrameValues[button];
    }

    bool GetButtonDown(int button)
    {
        AssertRelease(button < FKeyboardState.KeyStates.ThisFrameValues.size());
        return !FKeyboardState.AlreadyUsed && FKeyboardState.KeyStates.ThisFrameValues[button];
    }
    bool GetButtonHasChanged(int button)
    {
        AssertRelease(button < FKeyboardState.KeyStates.ThisFrameValues.size());
        return !FKeyboardState.AlreadyUsed && FKeyboardState.KeyStates.ThisFrameValues[button] != FKeyboardState.KeyStates.PreviousFrameValues[button];
    }

    bool IsShiftDown() { return !FKeyboardState.AlreadyUsed && FKeyboardState.IsShiftDown; }
    bool IsCtrlDown() { return !FKeyboardState.AlreadyUsed && FKeyboardState.IsCtrlDown; }
    bool IsAltDown() { return !FKeyboardState.AlreadyUsed && FKeyboardState.IsAltDown; }
    bool IsCapsLock() { return !FKeyboardState.AlreadyUsed && FKeyboardState.CapsLock; }

    void GamepadIsConnected(const int parGamepadId, const char* parGamepadName);
    void GamepadIsDisconnected(const int parGamepadId);
    bool IsGamepadConnected(const int parGamepadId);
    const char* GetGamepadName(const int parGamepadId);

    void SetGamepadState(const int parGamepadId, const uc8* parButtonsState, const float* parAxesStates);

    bool GetGamepadButtonDown(const int parGamepadId, GamepadButtons::Type parGamepadButton);
    bool GetGamepadButtonHasChanged(const int parGamepadId, GamepadButtons::Type parGamepadButton);
    float GetGamepadAxisValue(const int parGamepadId, GamepadAxes::Type parGamepadAxis);
    float GetGamepadAxisValueDelta(const int parGamepadId, GamepadAxes::Type parGamepadAxis);
    bool GetGamepadAxisValueHasChanged(const int parGamepadId, GamepadAxes::Type parGamepadAxis);

private:
    struct ButtonsState
    {
        std::vector<bool> ThisFrameValues;
        std::vector<bool> PreviousFrameValues;

        void Initialise(const u32 parNbKeys);
        void Swap();
        bool HasChangedDuringLastFrame();
    };

    struct KeyboardState
    {
        ButtonsState KeyStates;

        bool IsShiftDown = false;
        bool IsCtrlDown = false;
        bool IsAltDown = false;

        bool AlreadyUsed = false;

        bool CapsLock = false;

        bool HasChangedDuringLastFrame();
    };

    KeyboardState FKeyboardState;

    struct MouseState
    {
        vec2 MousePosition;
        vec2 PreviousMousePosition;
        vec2 MouseScrollDelta;
        vec2 PreviousMouseScrollDelta;

        ButtonsState MouseButtons;

        bool AlreadyUsed = false;

        void Initialise(const vec2& parMousePosition, const u32 parNbButtons);

        bool HasChangedDuringLastFrame();
    };

    MouseState FMouse;

    struct AxesState
    {
        std::vector<float> ThisFrameValues;
        std::vector<float> PreviousFrameValues;
        void Initialise(const u32 parNbAxes);
        void Swap();
        bool HasChangedDuringLastFrame();
    };

    struct Gamepad
    {
        const char* Name;
        ButtonsState GamepadButtonsStates;
        AxesState AxesStates;
        void Initialise();
        void Swap();
        bool HasChangedDuringLastFrame();
    };

    std::map<int, Gamepad> FGamepads;

    std::vector<u32> FTextInput;
};

namespace Input
{

void Initialise(const u32 parNbKeyboardKeys, const vec2& parMousePosition, const u32 parNbMouseButtons)
{
    InputManager::CreateIFP();
    InputManager::Instance().Initialise(parNbKeyboardKeys, parMousePosition, parNbMouseButtons);
}

void EndFrame()
{
    InputManager::Instance().EndFrame();
}

void Shutdown()
{
    InputManager::Instance().Shutdown();
    InputManager::Destroy();
}

void SetMousePosition(const vec2& parMousePosition)
{
    InputManager::Instance().SetMousePosition(parMousePosition);
}

void SetMouseScrollDelta(const vec2& parMouseScrollDelta)
{
    InputManager::Instance().SetMouseScrollDelta(parMouseScrollDelta);
}

void SetMouseButtonState(int button, bool value)
{
    InputManager::Instance().SetMouseButtonState(button, value);
}

void SetKeyboardButtonState(int button, bool value, bool isShiftDown, bool isCtrlDown, bool isAltDown)
{
    InputManager::Instance().SetKeyboardButtonState(button, value, isShiftDown, isCtrlDown, isAltDown);
}

void AddCharacterInput(u32 character)
{
    InputManager::Instance().AddCharacterInput(character);
}

bool TextInputHasChanged()
{
    return InputManager::Instance().TextInputHasChanged();
}

MemoryView<const u32> TextInput()
{
    return InputManager::Instance().TextInput();
}

void SetInputsAlreadyUsed(const bool parKeyboardInputUsed, const bool parMouseInputUsed)
{
    // LOG_INPUT(std::format("keyboard {} - mouse {}", ((parKeyboardInputUsed) ? "true" : "false"), ((parMouseInputUsed) ? "true" : "false")));
    InputManager::Instance().SetInputsAlreadyUsed(parKeyboardInputUsed, parMouseInputUsed);
}

void GetInputsAlreadyUsed(bool& parKeyboardInputUsed, bool& parMouseInputUsed)
{
    InputManager::Instance().GetInputsAlreadyUsed(parKeyboardInputUsed, parMouseInputUsed);
}

const vec2 GetMousePosition()
{
    return InputManager::Instance().GetMousePosition();
}

const vec2 GetMousePositionDelta()
{
    return InputManager::Instance().GetMousePositionDelta();
}

const vec2 GetMouseScrollDelta()
{
    return InputManager::Instance().GetMouseScrollDelta();
}

bool GetMouseButtonState(int button)
{
    return InputManager::Instance().GetMouseButtonState(button);
}

bool GetMouseButtonHasChanged(int button)
{
    return InputManager::Instance().GetMouseButtonHasChanged(button);
}

bool GetButtonDown(int button)
{
    return InputManager::Instance().GetButtonDown(button);
}

bool GetButtonHasChanged(int button)
{
    return InputManager::Instance().GetButtonHasChanged(button);
}

bool IsShiftDown()
{
    return InputManager::Instance().IsShiftDown();
}

bool IsCtrlDown()
{
    // ASSERTS !!!
    return InputManager::Instance().IsCtrlDown();
}

bool IsAltDown()
{
    return InputManager::Instance().IsAltDown();
}

bool NoSpecialKeysPressed()
{
    return !IsAltDown() && !IsCtrlDown() && !IsShiftDown();
}

bool IsCapsLock()
{
    return InputManager::Instance().IsCapsLock();
}

void GamepadIsConnected(const int parGamepadId, const char* parGamepadName)
{
    InputManager::Instance().GamepadIsConnected(parGamepadId, parGamepadName);
}

void GamepadIsDisconnected(const int parGamepadId)
{
    InputManager::Instance().GamepadIsDisconnected(parGamepadId);
}

bool IsGamepadConnected(const int parGamepadId)
{
    return InputManager::Instance().IsGamepadConnected(parGamepadId);
}

const char* GetGamepadName(const int parGamepadId)
{
    return InputManager::Instance().GetGamepadName(parGamepadId);
}

void SetGamepadState(const int parGamepadId, const uc8* parButtonsState, const float* parAxesStates)
{
    InputManager::Instance().SetGamepadState(parGamepadId, parButtonsState, parAxesStates);
}

bool GetGamepadButtonDown(const int parGamepadId, GamepadButtons::Type parGamepadButton)
{
    return InputManager::Instance().GetGamepadButtonDown(parGamepadId, parGamepadButton);
}

bool GetGamepadButtonHasChanged(const int parGamepadId, GamepadButtons::Type parGamepadButton)
{
    return InputManager::Instance().GetGamepadButtonHasChanged(parGamepadId, parGamepadButton);
}

float GetGamepadAxisValue(const int parGamepadId, GamepadAxes::Type parGamepadAxis)
{
    return InputManager::Instance().GetGamepadAxisValue(parGamepadId, parGamepadAxis);
}

float GetGamepadAxisValueDelta(const int parGamepadId, GamepadAxes::Type parGamepadAxis)
{
    return InputManager::Instance().GetGamepadAxisValueDelta(parGamepadId, parGamepadAxis);
}

bool GetGamepadAxisValueHasChanged(const int parGamepadId, GamepadAxes::Type parGamepadAxis)
{
    return InputManager::Instance().GetGamepadAxisValueHasChanged(parGamepadId, parGamepadAxis);
}

} // namespace Input

InputManager::InputManager()
    : Singleton()
{
}

InputManager::~InputManager()
{
}

void InputManager::Initialise(const u32 parNbKeyboardKeys, const vec2& parMousePosition, const u32 parNbMouseButtons)
{
    FKeyboardState.KeyStates.Initialise(parNbKeyboardKeys);
    FMouse.Initialise(parMousePosition, parNbMouseButtons);
}

void InputManager::EndFrame()
{
    // TODO --> Get events to fire

    // swap buffers;
    FKeyboardState.KeyStates.Swap();
    FMouse.PreviousMousePosition = FMouse.MousePosition;
    FMouse.PreviousMouseScrollDelta = FMouse.MouseScrollDelta;
    FMouse.MouseScrollDelta = vec2(0.f);
    FMouse.MouseButtons.Swap();
    FTextInput.clear();
}

void InputManager::Shutdown()
{
}

void InputManager::SetMousePosition(const vec2& parMousePosition)
{
    FMouse.MousePosition = parMousePosition;
}

void InputManager::SetMouseScrollDelta(const vec2& parMouseScrollDelta)
{
    FMouse.MouseScrollDelta = parMouseScrollDelta;
}

void InputManager::SetMouseButtonState(int button, bool value)
{
    AssertRelease(button < FMouse.MouseButtons.ThisFrameValues.size());
    FMouse.MouseButtons.ThisFrameValues[button] = value;
}

void InputManager::SetKeyboardButtonState(int button, bool value, bool isShiftDown, bool isCtrlDown, bool isAltDown)
{
    if (button == -1)
        return;

    AssertRelease(button < FKeyboardState.KeyStates.ThisFrameValues.size());
    FKeyboardState.KeyStates.ThisFrameValues[button] = value;
    FKeyboardState.IsShiftDown = isShiftDown;
    FKeyboardState.IsCtrlDown = isCtrlDown;
    FKeyboardState.IsAltDown = isAltDown;

    if (value && button == InputKeyNames::INPUT_KEY_CAPS_LOCK)
        FKeyboardState.CapsLock = !FKeyboardState.CapsLock;
}

void InputManager::GamepadIsConnected(const int parGamepadId, const char* parGamepadName)
{
    AssertRelease(FGamepads.find(parGamepadId) == FGamepads.end());
    Gamepad g;
    g.Name = parGamepadName;
    g.Initialise();
    FGamepads.insert_or_assign(parGamepadId, g);
}

void InputManager::GamepadIsDisconnected(const int parGamepadId)
{
    AlwaysCheckedAssert(FGamepads.find(parGamepadId) != FGamepads.end());
    FGamepads.erase(parGamepadId);
}

void InputManager::SetGamepadState(const int parGamepadId, const uc8* parButtonsState, const float* parAxesStates)
{
    AssertRelease(FGamepads.find(parGamepadId) != FGamepads.end());
    Gamepad& g = FGamepads.at(parGamepadId);
    forrange(i, 0, GamepadButtons::GAMEPAD_BUTTON_LAST) { g.GamepadButtonsStates.ThisFrameValues[i] = parButtonsState[i] == 1; }

    forrange(i, 0, GamepadAxes::GAMEPAD_AXIS_LAST) { g.AxesStates.ThisFrameValues[i] = parAxesStates[i]; }
}

bool InputManager::GetGamepadButtonDown(const int parGamepadId, GamepadButtons::Type parGamepadButton)
{
    AssertRelease(FGamepads.find(parGamepadId) != FGamepads.end());
    Gamepad& g = FGamepads.at(parGamepadId);
    return g.GamepadButtonsStates.ThisFrameValues[parGamepadButton];
}

bool InputManager::GetGamepadButtonHasChanged(const int parGamepadId, GamepadButtons::Type parGamepadButton)
{
    AssertRelease(FGamepads.find(parGamepadId) != FGamepads.end());
    Gamepad& g = FGamepads.at(parGamepadId);
    return g.GamepadButtonsStates.ThisFrameValues[parGamepadButton] != g.GamepadButtonsStates.PreviousFrameValues[parGamepadButton];
}

float InputManager::GetGamepadAxisValue(const int parGamepadId, GamepadAxes::Type parGamepadAxis)
{
    AssertRelease(FGamepads.find(parGamepadId) != FGamepads.end());
    Gamepad& g = FGamepads.at(parGamepadId);
    return g.AxesStates.ThisFrameValues[parGamepadAxis];
}

bool InputManager::GetGamepadAxisValueHasChanged(const int parGamepadId, GamepadAxes::Type parGamepadAxis)
{
    AssertRelease(FGamepads.find(parGamepadId) != FGamepads.end());
    Gamepad& g = FGamepads.at(parGamepadId);
    return g.AxesStates.ThisFrameValues[parGamepadAxis] != g.AxesStates.PreviousFrameValues[parGamepadAxis];
}

bool InputManager::IsGamepadConnected(const int parGamepadId)
{
    return FGamepads.find(parGamepadId) != FGamepads.end();
}

const char* InputManager::GetGamepadName(const int parGamepadId)
{
    AssertRelease(FGamepads.find(parGamepadId) != FGamepads.end());
    Gamepad& g = FGamepads.at(parGamepadId);
    return g.Name;
}

float InputManager::GetGamepadAxisValueDelta(const int parGamepadId, GamepadAxes::Type parGamepadAxis)
{
    AssertRelease(FGamepads.find(parGamepadId) != FGamepads.end());
    Gamepad& g = FGamepads.at(parGamepadId);
    return g.AxesStates.ThisFrameValues[parGamepadAxis] - g.AxesStates.PreviousFrameValues[parGamepadAxis];
}

void InputManager::SetInputsAlreadyUsed(const bool parKeyboardInputUsed, const bool parMouseInputUsed)
{
    FKeyboardState.AlreadyUsed = parKeyboardInputUsed;
    FMouse.AlreadyUsed = parMouseInputUsed;
}

void InputManager::GetInputsAlreadyUsed(bool& parKeyboardInputUsed, bool& parMouseInputUsed)
{
    parKeyboardInputUsed = FKeyboardState.AlreadyUsed;
    parMouseInputUsed = FMouse.AlreadyUsed;
}

void InputManager::AddCharacterInput(u32 character)
{
    if (character != 0)
        FTextInput.push_back(character);
}

bool InputManager::TextInputHasChanged() const
{
    return !FTextInput.empty();
}

MemoryView<const u32> InputManager::TextInput() const
{
    return MemoryView<const u32>(FTextInput.data(), FTextInput.size());
}

void InputManager::ButtonsState::Initialise(const u32 parNbKeys)
{
    ThisFrameValues.resize(parNbKeys, false);
    PreviousFrameValues.resize(parNbKeys, false);
}

void InputManager::ButtonsState::Swap()
{
    PreviousFrameValues = ThisFrameValues;
}

bool InputManager::ButtonsState::HasChangedDuringLastFrame()
{
    return ThisFrameValues != PreviousFrameValues;
}

bool InputManager::KeyboardState::HasChangedDuringLastFrame()
{
    return !AlreadyUsed && KeyStates.HasChangedDuringLastFrame();
}

void InputManager::MouseState::Initialise(const vec2& parMousePosition, const u32 parNbButtons)
{
    MousePosition = parMousePosition;
    PreviousMousePosition = MousePosition;
    MouseScrollDelta = vec2(0.0f);
    PreviousMouseScrollDelta = MouseScrollDelta;

    MouseButtons.Initialise(parNbButtons);
}

bool InputManager::MouseState::HasChangedDuringLastFrame()
{
    return !AlreadyUsed && (MouseButtons.HasChangedDuringLastFrame() || MousePosition != PreviousMousePosition || MouseScrollDelta != PreviousMouseScrollDelta);
}

void InputManager::AxesState::Initialise(const u32 parNbAxes)
{
    ThisFrameValues.resize(parNbAxes, 0.0f);
    PreviousFrameValues.resize(parNbAxes, 0.0f);
}

bool InputManager::AxesState::HasChangedDuringLastFrame()
{
    return ThisFrameValues != PreviousFrameValues;
}

void InputManager::Gamepad::Initialise()
{
    GamepadButtonsStates.Initialise(GamepadButtons::GAMEPAD_BUTTON_LAST);
    AxesStates.Initialise(GamepadAxes::GAMEPAD_AXIS_LAST);
}

bool InputManager::Gamepad::HasChangedDuringLastFrame()
{
    return GamepadButtonsStates.HasChangedDuringLastFrame() || AxesStates.HasChangedDuringLastFrame();
}

} // namespace ECSEngine
