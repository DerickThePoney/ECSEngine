#pragma once
#include "FixedSizedArray.h"
#include "Singleton.h"

namespace ECSEngine
{

namespace GamepadButtons
{
#define DECLARE_ENUM(NAME, ID) NAME = ID
enum Type
{
#include "GamepadButtons.inl"
};

#undef DECLARE_ENUM
const char* ToString(const Type parValue);
} // namespace GamepadButtons

namespace GamepadAxes
{
#define DECLARE_ENUM(NAME, ID) NAME = ID
enum Type
{
#include "GamepadAxes.inl"
};

#undef DECLARE_ENUM
const char* ToString(const Type parValue);
} // namespace GamepadAxes

namespace InputKeyNames
{
#define DECLARE_ENUM(NAME, ID) NAME = ID
enum Type
{
#include "InputKeyNames.inl"
};

#undef DECLARE_ENUM
const char* ToString(const Type parValue);
} // namespace InputKeyNames

namespace Input
{
void Initialise(const u32 parNbKeyboardKeys, const glm::vec2& parMousePosition, const u32 parNbMouseButtons);
void NewFrame();
void Shutdown();

void SetMousePosition(const glm::vec2& parMousePosition);
void SetMouseScrollDelta(const glm::vec2& parMouseScrollDelta);
void SetMouseButtonState(int button, bool value);
void SetKeyboardButtonState(int button, bool value, bool isShiftDown, bool isCtrlDown, bool isAltDown);

const glm::vec2& GetMousePosition();
const glm::vec2& GetMouseScrollDelta();
bool GetMouseButtonState(int button);

bool GetButtonDown(int button);

bool IsShiftDown();
bool IsCtrlDown();
bool IsAltDown();

void GamepadIsConnected(const int parGamepadId, const char* parGamepadName);
void GamepadIsDisconnected(const int parGamepadId);

bool IsGamepadConnected(const int parGamepadId);
const char* GetGamepadName(const int parGamepadId);

void SetGamepadState(const int parGamepadId, const uc8* parButtonsState, const float* parAxesStates);

bool GetGamepadButtonDown(const int parGamepadId, GamepadButtons::Type parGamepadButton);
float GetGamepadAxisValue(const int parGamepadId, GamepadAxes::Type parGamepadAxis);
} // namespace Input

} // namespace ECSEngine
