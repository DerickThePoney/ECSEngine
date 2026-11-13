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

void AssignPrintedLetter(InputKeyNames::Type* parPhysicalByLetter, InputKeyNames::Type parKey)
{
    const char* name = GetKeyName(parKey);
    if (name == nullptr || name[0] == '\0' || name[1] != '\0')
        return;

    const char printed = (char)std::tolower((unsigned char)name[0]);
    if (printed < 'a' || printed > 'z')
        return;

    parPhysicalByLetter[printed - 'a'] = parKey;
}

void RebuildLetterLayout(InputKeyNames::Type* parPhysicalByLetter)
{
    for (int letter = 0; letter < 26; ++letter)
        parPhysicalByLetter[letter] = (InputKeyNames::Type)(InputKeyNames::INPUT_KEY_A + letter);

    for (int key = InputKeyNames::INPUT_KEY_A; key <= InputKeyNames::INPUT_KEY_Z; ++key)
        AssignPrintedLetter(parPhysicalByLetter, (InputKeyNames::Type)key);

    // Letters can sit outside A-Z. AZERTY prints M on the US semicolon key.
    const InputKeyNames::Type extraPrintableKeys[] = {
        InputKeyNames::INPUT_KEY_SEMICOLON,
        InputKeyNames::INPUT_KEY_APOSTROPHE,
        InputKeyNames::INPUT_KEY_COMMA,
        InputKeyNames::INPUT_KEY_PERIOD,
        InputKeyNames::INPUT_KEY_SLASH,
        InputKeyNames::INPUT_KEY_MINUS,
        InputKeyNames::INPUT_KEY_EQUAL,
        InputKeyNames::INPUT_KEY_LEFT_BRACKET,
        InputKeyNames::INPUT_KEY_RIGHT_BRACKET,
        InputKeyNames::INPUT_KEY_BACKSLASH,
        InputKeyNames::INPUT_KEY_GRAVE_ACCENT,
        InputKeyNames::INPUT_KEY_WORLD_1,
        InputKeyNames::INPUT_KEY_WORLD_2,
    };
    for (InputKeyNames::Type key : extraPrintableKeys)
        AssignPrintedLetter(parPhysicalByLetter, key);
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
