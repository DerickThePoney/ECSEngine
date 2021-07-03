#include "stdafx.h"

#include "RmlSystemInterface.h"

#include "Common/InputManager.h"
#include "Common/TimeManager.h"

namespace ECSEngine
{
namespace UI
{

double RmlSystemInterface::GetElapsedTime()
{
    return (double)TimeManager::DurationSinceStartRealTime();
}

Rml::Input::KeyIdentifier RmlSystemInterface::ConvertToRml(InputKeyNames::Type parKey) const
{
    switch (parKey)
    {
#define DECLARE_ENUM(a, b)                                                                                                                                                         \
    case InputKeyNames::a:                                                                                                                                                         \
        return b;

        DECLARE_ENUM(INPUT_KEY_SPACE, Rml::Input::KeyIdentifier::KI_SPACE);
        DECLARE_ENUM(INPUT_KEY_APOSTROPHE, Rml::Input::KeyIdentifier::KI_4); /* ' */
        DECLARE_ENUM(INPUT_KEY_COMMA, Rml::Input::KeyIdentifier::KI_OEM_COMMA); /* , */
        DECLARE_ENUM(INPUT_KEY_MINUS, Rml::Input::KeyIdentifier::KI_OEM_MINUS); /* - */
        DECLARE_ENUM(INPUT_KEY_PERIOD, Rml::Input::KeyIdentifier::KI_OEM_PERIOD); /* . */
        DECLARE_ENUM(INPUT_KEY_SLASH, Rml::Input::KeyIdentifier::KI_OEM_2); /* / */
        DECLARE_ENUM(INPUT_KEY_0, Rml::Input::KeyIdentifier::KI_0);
        DECLARE_ENUM(INPUT_KEY_1, Rml::Input::KeyIdentifier::KI_1);
        DECLARE_ENUM(INPUT_KEY_2, Rml::Input::KeyIdentifier::KI_2);
        DECLARE_ENUM(INPUT_KEY_3, Rml::Input::KeyIdentifier::KI_3);
        DECLARE_ENUM(INPUT_KEY_4, Rml::Input::KeyIdentifier::KI_4);
        DECLARE_ENUM(INPUT_KEY_5, Rml::Input::KeyIdentifier::KI_5);
        DECLARE_ENUM(INPUT_KEY_6, Rml::Input::KeyIdentifier::KI_6);
        DECLARE_ENUM(INPUT_KEY_7, Rml::Input::KeyIdentifier::KI_7);
        DECLARE_ENUM(INPUT_KEY_8, Rml::Input::KeyIdentifier::KI_8);
        DECLARE_ENUM(INPUT_KEY_9, Rml::Input::KeyIdentifier::KI_9);
        DECLARE_ENUM(INPUT_KEY_SEMICOLON, Rml::Input::KeyIdentifier::KI_OEM_PERIOD); /* ; */
        DECLARE_ENUM(INPUT_KEY_EQUAL, Rml::Input::KeyIdentifier::KI_OEM_PLUS); /* , */
        DECLARE_ENUM(INPUT_KEY_A, Rml::Input::KeyIdentifier::KI_A);
        DECLARE_ENUM(INPUT_KEY_B, Rml::Input::KeyIdentifier::KI_B);
        DECLARE_ENUM(INPUT_KEY_C, Rml::Input::KeyIdentifier::KI_C);
        DECLARE_ENUM(INPUT_KEY_D, Rml::Input::KeyIdentifier::KI_D);
        DECLARE_ENUM(INPUT_KEY_E, Rml::Input::KeyIdentifier::KI_E);
        ;
        DECLARE_ENUM(INPUT_KEY_F, Rml::Input::KeyIdentifier::KI_F);
        DECLARE_ENUM(INPUT_KEY_G, Rml::Input::KeyIdentifier::KI_G);
        DECLARE_ENUM(INPUT_KEY_H, Rml::Input::KeyIdentifier::KI_H);
        DECLARE_ENUM(INPUT_KEY_I, Rml::Input::KeyIdentifier::KI_I);
        DECLARE_ENUM(INPUT_KEY_J, Rml::Input::KeyIdentifier::KI_J);
        DECLARE_ENUM(INPUT_KEY_K, Rml::Input::KeyIdentifier::KI_K);
        DECLARE_ENUM(INPUT_KEY_L, Rml::Input::KeyIdentifier::KI_L);
        DECLARE_ENUM(INPUT_KEY_M, Rml::Input::KeyIdentifier::KI_M);
        DECLARE_ENUM(INPUT_KEY_N, Rml::Input::KeyIdentifier::KI_N);
        DECLARE_ENUM(INPUT_KEY_O, Rml::Input::KeyIdentifier::KI_O);
        DECLARE_ENUM(INPUT_KEY_P, Rml::Input::KeyIdentifier::KI_P);
        DECLARE_ENUM(INPUT_KEY_Q, Rml::Input::KeyIdentifier::KI_Q);
        DECLARE_ENUM(INPUT_KEY_R, Rml::Input::KeyIdentifier::KI_R);
        DECLARE_ENUM(INPUT_KEY_S, Rml::Input::KeyIdentifier::KI_S);
        DECLARE_ENUM(INPUT_KEY_T, Rml::Input::KeyIdentifier::KI_T);
        DECLARE_ENUM(INPUT_KEY_U, Rml::Input::KeyIdentifier::KI_U);
        DECLARE_ENUM(INPUT_KEY_V, Rml::Input::KeyIdentifier::KI_V);
        DECLARE_ENUM(INPUT_KEY_W, Rml::Input::KeyIdentifier::KI_W);
        DECLARE_ENUM(INPUT_KEY_X, Rml::Input::KeyIdentifier::KI_X);
        DECLARE_ENUM(INPUT_KEY_Y, Rml::Input::KeyIdentifier::KI_Y);
        DECLARE_ENUM(INPUT_KEY_Z, Rml::Input::KeyIdentifier::KI_Z);
        DECLARE_ENUM(INPUT_KEY_LEFT_BRACKET, Rml::Input::KeyIdentifier::KI_5); /* [ */
        DECLARE_ENUM(INPUT_KEY_BACKSLASH, Rml::Input::KeyIdentifier::KI_8); /* \ */
        DECLARE_ENUM(INPUT_KEY_RIGHT_BRACKET, Rml::Input::KeyIdentifier::KI_OEM_4); /* ] */
        DECLARE_ENUM(INPUT_KEY_GRAVE_ACCENT, Rml::Input::KeyIdentifier::KI_2); /* ` */
        DECLARE_ENUM(INPUT_KEY_WORLD_1, Rml::Input::KeyIdentifier::KI_0); /* non-US #1 */
        DECLARE_ENUM(INPUT_KEY_WORLD_2, Rml::Input::KeyIdentifier::KI_0); /* non-US #2 */

        /* Function keys */
        DECLARE_ENUM(INPUT_KEY_ESCAPE, Rml::Input::KeyIdentifier::KI_ESCAPE);
        DECLARE_ENUM(INPUT_KEY_ENTER, Rml::Input::KeyIdentifier::KI_RETURN);
        DECLARE_ENUM(INPUT_KEY_TAB, Rml::Input::KeyIdentifier::KI_TAB);
        DECLARE_ENUM(INPUT_KEY_BACKSPACE, Rml::Input::KeyIdentifier::KI_BACK);
        DECLARE_ENUM(INPUT_KEY_INSERT, Rml::Input::KeyIdentifier::KI_INSERT);
        DECLARE_ENUM(INPUT_KEY_DELETE, Rml::Input::KeyIdentifier::KI_DELETE);
        DECLARE_ENUM(INPUT_KEY_RIGHT, Rml::Input::KeyIdentifier::KI_RIGHT);
        DECLARE_ENUM(INPUT_KEY_LEFT, Rml::Input::KeyIdentifier::KI_LEFT);
        DECLARE_ENUM(INPUT_KEY_DOWN, Rml::Input::KeyIdentifier::KI_DOWN);
        DECLARE_ENUM(INPUT_KEY_UP, Rml::Input::KeyIdentifier::KI_UP);
        DECLARE_ENUM(INPUT_KEY_PAGE_UP, Rml::Input::KeyIdentifier::KI_PRIOR);
        DECLARE_ENUM(INPUT_KEY_PAGE_DOWN, Rml::Input::KeyIdentifier::KI_NEXT);
        DECLARE_ENUM(INPUT_KEY_HOME, Rml::Input::KeyIdentifier::KI_HOME);
        DECLARE_ENUM(INPUT_KEY_END, Rml::Input::KeyIdentifier::KI_END);
        DECLARE_ENUM(INPUT_KEY_CAPS_LOCK, Rml::Input::KeyIdentifier::KI_CAPITAL);
        DECLARE_ENUM(INPUT_KEY_SCROLL_LOCK, Rml::Input::KeyIdentifier::KI_SCROLL);
        DECLARE_ENUM(INPUT_KEY_NUM_LOCK, Rml::Input::KeyIdentifier::KI_NUMLOCK);
        DECLARE_ENUM(INPUT_KEY_PRINT_SCREEN, Rml::Input::KeyIdentifier::KI_PRINT);
        DECLARE_ENUM(INPUT_KEY_PAUSE, Rml::Input::KeyIdentifier::KI_PAUSE);
        DECLARE_ENUM(INPUT_KEY_F1, Rml::Input::KeyIdentifier::KI_F1);
        DECLARE_ENUM(INPUT_KEY_F2, Rml::Input::KeyIdentifier::KI_F2);
        DECLARE_ENUM(INPUT_KEY_F3, Rml::Input::KeyIdentifier::KI_F3);
        DECLARE_ENUM(INPUT_KEY_F4, Rml::Input::KeyIdentifier::KI_F4);
        DECLARE_ENUM(INPUT_KEY_F5, Rml::Input::KeyIdentifier::KI_F5);
        DECLARE_ENUM(INPUT_KEY_F6, Rml::Input::KeyIdentifier::KI_F6);
        DECLARE_ENUM(INPUT_KEY_F7, Rml::Input::KeyIdentifier::KI_F7);
        DECLARE_ENUM(INPUT_KEY_F8, Rml::Input::KeyIdentifier::KI_F8);
        DECLARE_ENUM(INPUT_KEY_F9, Rml::Input::KeyIdentifier::KI_F9);
        DECLARE_ENUM(INPUT_KEY_F10, Rml::Input::KeyIdentifier::KI_F10);
        DECLARE_ENUM(INPUT_KEY_F11, Rml::Input::KeyIdentifier::KI_F11);
        DECLARE_ENUM(INPUT_KEY_F12, Rml::Input::KeyIdentifier::KI_F12);
        DECLARE_ENUM(INPUT_KEY_F13, Rml::Input::KeyIdentifier::KI_F13);
        DECLARE_ENUM(INPUT_KEY_F14, Rml::Input::KeyIdentifier::KI_F14);
        DECLARE_ENUM(INPUT_KEY_F15, Rml::Input::KeyIdentifier::KI_F15);
        DECLARE_ENUM(INPUT_KEY_F16, Rml::Input::KeyIdentifier::KI_F16);
        DECLARE_ENUM(INPUT_KEY_F17, Rml::Input::KeyIdentifier::KI_F17);
        DECLARE_ENUM(INPUT_KEY_F18, Rml::Input::KeyIdentifier::KI_F18);
        DECLARE_ENUM(INPUT_KEY_F19, Rml::Input::KeyIdentifier::KI_F19);
        DECLARE_ENUM(INPUT_KEY_F20, Rml::Input::KeyIdentifier::KI_F20);
        DECLARE_ENUM(INPUT_KEY_F21, Rml::Input::KeyIdentifier::KI_F21);
        DECLARE_ENUM(INPUT_KEY_F22, Rml::Input::KeyIdentifier::KI_F22);
        DECLARE_ENUM(INPUT_KEY_F23, Rml::Input::KeyIdentifier::KI_F23);
        DECLARE_ENUM(INPUT_KEY_F24, Rml::Input::KeyIdentifier::KI_F24);
        DECLARE_ENUM(INPUT_KEY_KP_0, Rml::Input::KeyIdentifier::KI_NUMPAD0);
        DECLARE_ENUM(INPUT_KEY_KP_1, Rml::Input::KeyIdentifier::KI_NUMPAD1);
        DECLARE_ENUM(INPUT_KEY_KP_2, Rml::Input::KeyIdentifier::KI_NUMPAD2);
        DECLARE_ENUM(INPUT_KEY_KP_3, Rml::Input::KeyIdentifier::KI_NUMPAD3);
        DECLARE_ENUM(INPUT_KEY_KP_4, Rml::Input::KeyIdentifier::KI_NUMPAD4);
        DECLARE_ENUM(INPUT_KEY_KP_5, Rml::Input::KeyIdentifier::KI_NUMPAD5);
        DECLARE_ENUM(INPUT_KEY_KP_6, Rml::Input::KeyIdentifier::KI_NUMPAD6);
        DECLARE_ENUM(INPUT_KEY_KP_7, Rml::Input::KeyIdentifier::KI_NUMPAD7);
        DECLARE_ENUM(INPUT_KEY_KP_8, Rml::Input::KeyIdentifier::KI_NUMPAD8);
        DECLARE_ENUM(INPUT_KEY_KP_9, Rml::Input::KeyIdentifier::KI_NUMPAD9);
        DECLARE_ENUM(INPUT_KEY_KP_DECIMAL, Rml::Input::KeyIdentifier::KI_DECIMAL);
        DECLARE_ENUM(INPUT_KEY_KP_DIVIDE, Rml::Input::KeyIdentifier::KI_DIVIDE);
        DECLARE_ENUM(INPUT_KEY_KP_MULTIPLY, Rml::Input::KeyIdentifier::KI_MULTIPLY);
        DECLARE_ENUM(INPUT_KEY_KP_SUBTRACT, Rml::Input::KeyIdentifier::KI_SUBTRACT);
        DECLARE_ENUM(INPUT_KEY_KP_ADD, Rml::Input::KeyIdentifier::KI_ADD);
        DECLARE_ENUM(INPUT_KEY_KP_ENTER, Rml::Input::KeyIdentifier::KI_NUMPADENTER);
        DECLARE_ENUM(INPUT_KEY_KP_EQUAL, Rml::Input::KeyIdentifier::KI_OEM_PLUS);
        DECLARE_ENUM(INPUT_KEY_LEFT_SHIFT, Rml::Input::KeyIdentifier::KI_LSHIFT);
        DECLARE_ENUM(INPUT_KEY_LEFT_CONTROL, Rml::Input::KeyIdentifier::KI_LCONTROL);
        DECLARE_ENUM(INPUT_KEY_LEFT_ALT, Rml::Input::KeyIdentifier::KI_LMENU);
        // DECLARE_ENUM(INPUT_KEY_LEFT_SUPER, Rml::Input::KeyIdentifier::KI_NUMPAD0);
        DECLARE_ENUM(INPUT_KEY_RIGHT_SHIFT, Rml::Input::KeyIdentifier::KI_RSHIFT);
        DECLARE_ENUM(INPUT_KEY_RIGHT_CONTROL, Rml::Input::KeyIdentifier::KI_RCONTROL);
        DECLARE_ENUM(INPUT_KEY_RIGHT_ALT, Rml::Input::KeyIdentifier::KI_RMENU);
        // DECLARE_ENUM(INPUT_KEY_RIGHT_SUPER, Rml::Input::KeyIdentifier::KI_NUMPAD0);
        DECLARE_ENUM(INPUT_KEY_MENU, Rml::Input::KeyIdentifier::KI_LWIN);
#undef DECLARE_ENUM
    }

    return Rml::Input::KeyIdentifier::KI_UNKNOWN;
}

} // namespace UI
} // namespace ECSEngine