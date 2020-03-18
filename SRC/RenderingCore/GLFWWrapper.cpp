#include "stdafx.h"

#include "GLFWWrapper.h"

#include "GLFW/glfw3.h"
namespace ECSEngine
{
namespace GLFWWrapper
{

const char* GetKeyName(const InputKeyNames::Type parKeyName)
{
    return glfwGetKeyName(GLFW_KEY_UNKNOWN, glfwGetKeyScancode(parKeyName));
}

} // namespace GLFWWrapper
} // namespace ECSEngine