#pragma once
#include "Common/InputManager.h"

namespace ECSEngine
{
namespace GLFWWrapper
{
const char* GetKeyName(const InputKeyNames::Type parKeyName);

// Letter bindings follow the printed key. GLFW key codes are US positions, so on AZERTY the code for W is the key labeled Z.
InputKeyNames::Type PhysicalKeyForBinding(InputKeyNames::Type parKey);
}
} // namespace ECSEngine
