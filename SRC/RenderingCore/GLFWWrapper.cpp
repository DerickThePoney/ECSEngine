#include "stdafx.h"

#include "GLFWWrapper.h"

#include "GLFW/glfw3.h"

#include <cctype>
namespace ECSEngine
{
namespace GLFWWrapper
{

const char* GetKeyName(const InputKeyNames::Type parKeyName)
{
    return glfwGetKeyName(GLFW_KEY_UNKNOWN, glfwGetKeyScancode(parKeyName));
}

namespace
{
bool PrintedNameIs(const char* parName, char parLetter)
{
    return parName != nullptr && parName[0] != '\0' && parName[1] == '\0'
        && (char)std::tolower((unsigned char)parName[0]) == parLetter;
}

void RebuildLetterLayout(InputKeyNames::Type* parPhysicalByLetter)
{
    for (int letter = 0; letter < 26; ++letter)
        parPhysicalByLetter[letter] = (InputKeyNames::Type)(InputKeyNames::INPUT_KEY_A + letter);

    for (int key = InputKeyNames::INPUT_KEY_A; key <= InputKeyNames::INPUT_KEY_Z; ++key)
    {
        const char* name = GetKeyName((InputKeyNames::Type)key);
        if (name == nullptr || name[0] == '\0' || name[1] != '\0')
            continue;

        const char printed = (char)std::tolower((unsigned char)name[0]);
        if (printed < 'a' || printed > 'z')
            continue;

        parPhysicalByLetter[printed - 'a'] = (InputKeyNames::Type)key;
    }
}
} // namespace

InputKeyNames::Type PhysicalKeyForBinding(InputKeyNames::Type parKey)
{
    if (parKey < InputKeyNames::INPUT_KEY_A || parKey > InputKeyNames::INPUT_KEY_Z)
        return parKey;

    static bool ready = false;
    static InputKeyNames::Type physicalByLetter[26];
    const int index = parKey - InputKeyNames::INPUT_KEY_A;
    const char expected = (char)('a' + index);
    if (!ready || !PrintedNameIs(GetKeyName(physicalByLetter[index]), expected))
    {
        RebuildLetterLayout(physicalByLetter);
        ready = true;
    }

    return physicalByLetter[index];
}

} // namespace GLFWWrapper
} // namespace ECSEngine
