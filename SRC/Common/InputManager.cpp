#include "stdafx.h"

#include "InputManager.h"

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

class InputManager final : public Singleton<InputManager>
{
public:
    InputManager();
    ~InputManager();

    void Initialise(const u32 parNbKeyboardKeys, const glm::vec2& parMousePosition, const u32 parNbMouseButtons);
    void NewFrame();
    void Shutdown();

    void SetMousePosition(const glm::vec2& parMousePosition);
    void SetMouseScrollDelta(const glm::vec2& parMouseScrollDelta);
    void SetMouseButtonState(int button, bool value);
    void SetKeyboardButtonState(int button, bool value, bool isShiftDown, bool isCtrlDown, bool isAltDown);

    const glm::vec2& GetMousePosition() { return FMouse.MousePosition; }
    const glm::vec2& GetMouseScrollDelta() { return FMouse.MouseScrollDelta; }
    bool GetMouseButtonState(int button)
    {
        AssertRelease(button < FMouse.MouseButtons.ThisFrameValues.size());
        return FMouse.MouseButtons.ThisFrameValues[button];
    }

    bool GetButtonDown(int button)
    {
        AssertRelease(button < FKeyboardState.KeyStates.ThisFrameValues.size());
        return FKeyboardState.KeyStates.ThisFrameValues[button];
    }

    bool IsShiftDown() { return FKeyboardState.IsShiftDown; }
    bool IsCtrlDown() { return FKeyboardState.IsCtrlDown; }
    bool IsAltDown() { return FKeyboardState.IsAltDown; }

    void GamepadIsConnected(const int parGamepadId, const char* parGamepadName);
    void GamepadIsDisconnected(const int parGamepadId);
    bool IsGamepadConnected(const int parGamepadId);
    const char* GetGamepadName(const int parGamepadId);

    void SetGamepadState(const int parGamepadId, const uc8* parButtonsState, const float* parAxesStates);

    bool GetGamepadButtonDown(const int parGamepadId, GamepadButtons::Type parGamepadButton);
    float GetGamepadAxisValue(const int parGamepadId, GamepadAxes::Type parGamepadAxis);

private:
    struct ButtonsState
    {
        std::vector<bool> ThisFrameValues;
        std::vector<bool> PreviousFrameValues;

        void Initialise(const u32 parNbKeys);
        void Swap();
    };

    struct KeyboardState
    {
        ButtonsState KeyStates;

        bool IsShiftDown = false;
        bool IsCtrlDown = false;
        bool IsAltDown = false;
    };

    KeyboardState FKeyboardState;

    struct MouseState
    {
        glm::vec2 MousePosition;
        glm::vec2 PreviousMousePosition;
        glm::vec2 MouseScrollDelta;
        glm::vec2 PreviousMouseScrollDelta;

        ButtonsState MouseButtons;

        void Initialise(const glm::vec2& parMousePosition, const u32 parNbButtons);
    };

    MouseState FMouse;

    struct AxesState
    {
        std::vector<float> ThisFrameValues;
        std::vector<float> PreviousFrameValues;
        void Initialise(const u32 parNbAxes);
        void Swap();
    };

    struct Gamepad
    {
        const char* Name;
        ButtonsState GamepadButtonsStates;
        AxesState AxesStates;
        void Initialise();
        void Swap();
    };

    std::map<int, Gamepad> FGamepads;
};

namespace Input
{

void Initialise(const u32 parNbKeyboardKeys, const glm::vec2& parMousePosition, const u32 parNbMouseButtons)
{
    InputManager::CreateIFP();
    InputManager::Instance().Initialise(parNbKeyboardKeys, parMousePosition, parNbMouseButtons);
}

void NewFrame()
{
    InputManager::Instance().NewFrame();
}

void Shutdown()
{
    InputManager::Instance().Shutdown();
    InputManager::Destroy();
}

void SetMousePosition(const glm::vec2& parMousePosition)
{
    InputManager::Instance().SetMousePosition(parMousePosition);
}

void SetMouseScrollDelta(const glm::vec2& parMouseScrollDelta)
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

const glm::vec2& GetMousePosition()
{
    return InputManager::Instance().GetMousePosition();
}

const glm::vec2& GetMouseScrollDelta()
{
    return InputManager::Instance().GetMouseScrollDelta();
}

bool GetMouseButtonState(int button)
{
    return InputManager::Instance().GetMouseButtonState(button);
}

bool GetButtonDown(int button)
{
    return InputManager::Instance().GetButtonDown(button);
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

float GetGamepadAxisValue(const int parGamepadId, GamepadAxes::Type parGamepadAxis)
{
    return InputManager::Instance().GetGamepadAxisValue(parGamepadId, parGamepadAxis);
}

} // namespace Input

InputManager::InputManager()
    : Singleton()
{
}

InputManager::~InputManager()
{
}

void InputManager::Initialise(const u32 parNbKeyboardKeys, const glm::vec2& parMousePosition, const u32 parNbMouseButtons)
{
    FKeyboardState.KeyStates.Initialise(parNbKeyboardKeys);
    FMouse.Initialise(parMousePosition, parNbMouseButtons);
}

void InputManager::NewFrame()
{
    // TODO --> Get events to fire

    // swap buffers;
    FKeyboardState.KeyStates.Swap();
    FMouse.PreviousMousePosition = FMouse.MousePosition;
    FMouse.PreviousMouseScrollDelta = FMouse.MouseScrollDelta;
    FMouse.MouseScrollDelta = glm::vec2(0.f);
    FMouse.MouseButtons.Swap();
}

void InputManager::Shutdown()
{
}

void InputManager::SetMousePosition(const glm::vec2& parMousePosition)
{
    FMouse.MousePosition = parMousePosition;
}

void InputManager::SetMouseScrollDelta(const glm::vec2& parMouseScrollDelta)
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

float InputManager::GetGamepadAxisValue(const int parGamepadId, GamepadAxes::Type parGamepadAxis)
{
    AssertRelease(FGamepads.find(parGamepadId) != FGamepads.end());
    Gamepad& g = FGamepads.at(parGamepadId);
    return g.AxesStates.ThisFrameValues[parGamepadAxis];
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

void InputManager::ButtonsState::Initialise(const u32 parNbKeys)
{
    ThisFrameValues.resize(parNbKeys, false);
    PreviousFrameValues.resize(parNbKeys, false);
}

void InputManager::ButtonsState::Swap()
{
    PreviousFrameValues = ThisFrameValues;
}

void InputManager::MouseState::Initialise(const glm::vec2& parMousePosition, const u32 parNbButtons)
{
    MousePosition = parMousePosition;
    PreviousMousePosition = MousePosition;
    MouseScrollDelta = glm::vec2(0.0f);
    PreviousMouseScrollDelta = MouseScrollDelta;

    MouseButtons.Initialise(parNbButtons);
}

void InputManager::AxesState::Initialise(const u32 parNbAxes)
{
    ThisFrameValues.resize(parNbAxes, 0.0f);
    PreviousFrameValues.resize(parNbAxes, 0.0f);
}

void InputManager::Gamepad::Initialise()
{
    GamepadButtonsStates.Initialise(GamepadButtons::GAMEPAD_BUTTON_LAST);
    AxesStates.Initialise(GamepadAxes::GAMEPAD_AXIS_LAST);
}

} // namespace ECSEngine