#include "stdafx.h"

#include "GLFWDisplayWindowHandler.h"
//clang-format off
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
//clang-format on

#include "BGFXRenderingBackend.h"
#include "Common/InputManager.h"
#include "ImguiRenderer.h"

namespace ECSEngine
{
namespace Rendering
{

namespace
{
ImGuiKey GlfwKeyToImGuiKey(int keycode)
{
    switch (keycode)
    {
    case GLFW_KEY_TAB:
        return ImGuiKey_Tab;
    case GLFW_KEY_LEFT:
        return ImGuiKey_LeftArrow;
    case GLFW_KEY_RIGHT:
        return ImGuiKey_RightArrow;
    case GLFW_KEY_UP:
        return ImGuiKey_UpArrow;
    case GLFW_KEY_DOWN:
        return ImGuiKey_DownArrow;
    case GLFW_KEY_PAGE_UP:
        return ImGuiKey_PageUp;
    case GLFW_KEY_PAGE_DOWN:
        return ImGuiKey_PageDown;
    case GLFW_KEY_HOME:
        return ImGuiKey_Home;
    case GLFW_KEY_END:
        return ImGuiKey_End;
    case GLFW_KEY_INSERT:
        return ImGuiKey_Insert;
    case GLFW_KEY_DELETE:
        return ImGuiKey_Delete;
    case GLFW_KEY_BACKSPACE:
        return ImGuiKey_Backspace;
    case GLFW_KEY_SPACE:
        return ImGuiKey_Space;
    case GLFW_KEY_ENTER:
        return ImGuiKey_Enter;
    case GLFW_KEY_ESCAPE:
        return ImGuiKey_Escape;
    case GLFW_KEY_APOSTROPHE:
        return ImGuiKey_Apostrophe;
    case GLFW_KEY_COMMA:
        return ImGuiKey_Comma;
    case GLFW_KEY_MINUS:
        return ImGuiKey_Minus;
    case GLFW_KEY_PERIOD:
        return ImGuiKey_Period;
    case GLFW_KEY_SLASH:
        return ImGuiKey_Slash;
    case GLFW_KEY_SEMICOLON:
        return ImGuiKey_Semicolon;
    case GLFW_KEY_EQUAL:
        return ImGuiKey_Equal;
    case GLFW_KEY_LEFT_BRACKET:
        return ImGuiKey_LeftBracket;
    case GLFW_KEY_BACKSLASH:
        return ImGuiKey_Backslash;
    case GLFW_KEY_RIGHT_BRACKET:
        return ImGuiKey_RightBracket;
    case GLFW_KEY_GRAVE_ACCENT:
        return ImGuiKey_GraveAccent;
    case GLFW_KEY_CAPS_LOCK:
        return ImGuiKey_CapsLock;
    case GLFW_KEY_SCROLL_LOCK:
        return ImGuiKey_ScrollLock;
    case GLFW_KEY_NUM_LOCK:
        return ImGuiKey_NumLock;
    case GLFW_KEY_PRINT_SCREEN:
        return ImGuiKey_PrintScreen;
    case GLFW_KEY_PAUSE:
        return ImGuiKey_Pause;
    case GLFW_KEY_KP_0:
        return ImGuiKey_Keypad0;
    case GLFW_KEY_KP_1:
        return ImGuiKey_Keypad1;
    case GLFW_KEY_KP_2:
        return ImGuiKey_Keypad2;
    case GLFW_KEY_KP_3:
        return ImGuiKey_Keypad3;
    case GLFW_KEY_KP_4:
        return ImGuiKey_Keypad4;
    case GLFW_KEY_KP_5:
        return ImGuiKey_Keypad5;
    case GLFW_KEY_KP_6:
        return ImGuiKey_Keypad6;
    case GLFW_KEY_KP_7:
        return ImGuiKey_Keypad7;
    case GLFW_KEY_KP_8:
        return ImGuiKey_Keypad8;
    case GLFW_KEY_KP_9:
        return ImGuiKey_Keypad9;
    case GLFW_KEY_KP_DECIMAL:
        return ImGuiKey_KeypadDecimal;
    case GLFW_KEY_KP_DIVIDE:
        return ImGuiKey_KeypadDivide;
    case GLFW_KEY_KP_MULTIPLY:
        return ImGuiKey_KeypadMultiply;
    case GLFW_KEY_KP_SUBTRACT:
        return ImGuiKey_KeypadSubtract;
    case GLFW_KEY_KP_ADD:
        return ImGuiKey_KeypadAdd;
    case GLFW_KEY_KP_ENTER:
        return ImGuiKey_KeypadEnter;
    case GLFW_KEY_KP_EQUAL:
        return ImGuiKey_KeypadEqual;
    case GLFW_KEY_LEFT_SHIFT:
        return ImGuiKey_LeftShift;
    case GLFW_KEY_LEFT_CONTROL:
        return ImGuiKey_LeftCtrl;
    case GLFW_KEY_LEFT_ALT:
        return ImGuiKey_LeftAlt;
    case GLFW_KEY_LEFT_SUPER:
        return ImGuiKey_LeftSuper;
    case GLFW_KEY_RIGHT_SHIFT:
        return ImGuiKey_RightShift;
    case GLFW_KEY_RIGHT_CONTROL:
        return ImGuiKey_RightCtrl;
    case GLFW_KEY_RIGHT_ALT:
        return ImGuiKey_RightAlt;
    case GLFW_KEY_RIGHT_SUPER:
        return ImGuiKey_RightSuper;
    case GLFW_KEY_MENU:
        return ImGuiKey_Menu;
    case GLFW_KEY_0:
        return ImGuiKey_0;
    case GLFW_KEY_1:
        return ImGuiKey_1;
    case GLFW_KEY_2:
        return ImGuiKey_2;
    case GLFW_KEY_3:
        return ImGuiKey_3;
    case GLFW_KEY_4:
        return ImGuiKey_4;
    case GLFW_KEY_5:
        return ImGuiKey_5;
    case GLFW_KEY_6:
        return ImGuiKey_6;
    case GLFW_KEY_7:
        return ImGuiKey_7;
    case GLFW_KEY_8:
        return ImGuiKey_8;
    case GLFW_KEY_9:
        return ImGuiKey_9;
    case GLFW_KEY_A:
        return ImGuiKey_A;
    case GLFW_KEY_B:
        return ImGuiKey_B;
    case GLFW_KEY_C:
        return ImGuiKey_C;
    case GLFW_KEY_D:
        return ImGuiKey_D;
    case GLFW_KEY_E:
        return ImGuiKey_E;
    case GLFW_KEY_F:
        return ImGuiKey_F;
    case GLFW_KEY_G:
        return ImGuiKey_G;
    case GLFW_KEY_H:
        return ImGuiKey_H;
    case GLFW_KEY_I:
        return ImGuiKey_I;
    case GLFW_KEY_J:
        return ImGuiKey_J;
    case GLFW_KEY_K:
        return ImGuiKey_K;
    case GLFW_KEY_L:
        return ImGuiKey_L;
    case GLFW_KEY_M:
        return ImGuiKey_M;
    case GLFW_KEY_N:
        return ImGuiKey_N;
    case GLFW_KEY_O:
        return ImGuiKey_O;
    case GLFW_KEY_P:
        return ImGuiKey_P;
    case GLFW_KEY_Q:
        return ImGuiKey_Q;
    case GLFW_KEY_R:
        return ImGuiKey_R;
    case GLFW_KEY_S:
        return ImGuiKey_S;
    case GLFW_KEY_T:
        return ImGuiKey_T;
    case GLFW_KEY_U:
        return ImGuiKey_U;
    case GLFW_KEY_V:
        return ImGuiKey_V;
    case GLFW_KEY_W:
        return ImGuiKey_W;
    case GLFW_KEY_X:
        return ImGuiKey_X;
    case GLFW_KEY_Y:
        return ImGuiKey_Y;
    case GLFW_KEY_Z:
        return ImGuiKey_Z;
    case GLFW_KEY_F1:
        return ImGuiKey_F1;
    case GLFW_KEY_F2:
        return ImGuiKey_F2;
    case GLFW_KEY_F3:
        return ImGuiKey_F3;
    case GLFW_KEY_F4:
        return ImGuiKey_F4;
    case GLFW_KEY_F5:
        return ImGuiKey_F5;
    case GLFW_KEY_F6:
        return ImGuiKey_F6;
    case GLFW_KEY_F7:
        return ImGuiKey_F7;
    case GLFW_KEY_F8:
        return ImGuiKey_F8;
    case GLFW_KEY_F9:
        return ImGuiKey_F9;
    case GLFW_KEY_F10:
        return ImGuiKey_F10;
    case GLFW_KEY_F11:
        return ImGuiKey_F11;
    case GLFW_KEY_F12:
        return ImGuiKey_F12;
    default:
        return ImGuiKey_None;
    }
}

void UpdateImGuiKeyModifiers(ImGuiIO& io, GLFWwindow* window)
{
    io.AddKeyEvent(ImGuiMod_Ctrl, (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS));
    io.AddKeyEvent(ImGuiMod_Shift, (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS));
    io.AddKeyEvent(ImGuiMod_Alt, (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS));
    io.AddKeyEvent(ImGuiMod_Super, (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS));
}

void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    AssertRelease(GLFWDisplayWindowHandler::HasInstance());
    GLFWDisplayWindowHandler::Instance().ResizeWindow(window, width, height);
    AssertRelease(BGFXRenderingBackend::HasInstance());
    BGFXRenderingBackend::Instance().Resize(width, height);
}

void WindowScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    for (u16 i = (u16)RenderPassId::IMGUI_PASSES_START; i < (u16)RenderPassId::IMGUI_PASSES_END + 1; i++)
    {
        ImGUI::SetImGuiContext((RenderPassId::Type)i);
        ImGui::GetIO().AddMouseWheelEvent((float)xoffset, (float)yoffset);
    }

    Input::SetMouseScrollDelta(vec2((float)xoffset, (float)yoffset));
}

void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_RELEASE)
        return;

    const bool down = (action == GLFW_PRESS);
    const ImGuiKey imguiKey = GlfwKeyToImGuiKey(key);
    bool ctrl = false, shift = false, alt = false;

    for (u16 i = (u16)RenderPassId::IMGUI_PASSES_START; i < (u16)RenderPassId::IMGUI_PASSES_END + 1; i++)
    {
        ImGUI::SetImGuiContext((RenderPassId::Type)i);
        ImGuiIO& io = ImGui::GetIO();
        UpdateImGuiKeyModifiers(io, window);
        if (imguiKey != ImGuiKey_None)
        {
            io.AddKeyEvent(imguiKey, down);
            io.SetKeyEventNativeData(imguiKey, key, scancode);
        }
        ctrl = ctrl || io.KeyCtrl;
        shift = shift || io.KeyShift;
        alt = alt || io.KeyAlt;
    }

    Input::SetKeyboardButtonState(key, down, shift, ctrl, alt);
}

void WindowCharCallback(GLFWwindow* window, unsigned int c)
{
    for (u16 i = (u16)RenderPassId::IMGUI_PASSES_START; i < (u16)RenderPassId::IMGUI_PASSES_END + 1; i++)
    {
        ImGUI::SetImGuiContext((RenderPassId::Type)i);
        ImGui::GetIO().AddInputCharacter(c);
    }

    Input::AddCharacterInput(c);
}

void WindowMousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Input::SetMousePosition(vec2((float)xpos, (float)ypos));
}

void WindowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Input::SetMouseButtonState(button, action == GLFW_PRESS);
}

void WindowJoystickCallback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        std::cout << glfwGetJoystickName(jid) << " is connected with id " << jid << std::endl;

        Input::GamepadIsConnected(jid, glfwGetJoystickName(jid));
    }
    else if (event == GLFW_DISCONNECTED)
    {
        const char* joystickName = glfwGetJoystickName(jid);
        if (joystickName != nullptr)
        {
            std::cout << joystickName;
        }
        std::cout << " is disconnected with id " << jid << std::endl;

        Input::GamepadIsDisconnected(jid);
    }
}

void InitJoysticks()
{
    forrange(i, GLFW_JOYSTICK_1, GLFW_JOYSTICK_LAST + 1)
    {
        int present = glfwJoystickPresent((int)i);
        if (present == GLFW_TRUE && glfwJoystickIsGamepad((int)i) == GLFW_TRUE)
        {
            std::cout << "Joystick " << i << " is present : " << glfwGetJoystickName((int)i) << std::endl;

            Input::GamepadIsConnected((int)i, glfwGetJoystickName((int)i));
        }
    }
}

void UpdateGamepads()
{
    forrange(i, GLFW_JOYSTICK_1, GLFW_JOYSTICK_LAST + 1)
    {
        int present = glfwJoystickPresent((int)i);
        if (present == GLFW_TRUE && glfwJoystickIsGamepad((int)i) == GLFW_TRUE)
        {
            GLFWgamepadstate state;
            glfwGetGamepadState((int)i, &state);
            Input::SetGamepadState((int)i, state.buttons, state.axes);
        }
    }
}

} // namespace

GLFWDisplayWindowHandler::GLFWDisplayWindowHandler()
    : FWindow(nullptr)
    , FWidth(1280)
    , FHeight(720)
    , FName("DEFAULT_NAME_CHANGE_IT_OR DIE!!!")
    , FMouseCursors()
{
    FMouseCursors = new GLFWcursor*[ImGuiMouseCursor_COUNT];
}

GLFWDisplayWindowHandler::~GLFWDisplayWindowHandler()
{
    AlwaysCheckedAssert(FWindow == nullptr);
    if (FWindow != nullptr)
        Destroy();

    delete[] FMouseCursors;
}

void GLFWDisplayWindowHandler::Init()
{
    glfwInit();

    AlwaysCheckedAssert(FWindow == nullptr);
    if (FWindow != nullptr)
        Destroy();

    GLFWmonitor* monitor = nullptr;

#ifdef COMPILE_FINAL
    monitor = glfwGetPrimaryMonitor();
    AssertRelease(monitor != nullptr);
    const GLFWvidmode* resolution = glfwGetVideoMode(monitor);
    AssertRelease(resolution != nullptr);
    FWidth = resolution->width;
    FHeight = resolution->height;
#endif

    FWindow = glfwCreateWindow(FWidth, FHeight, FName.c_str(), monitor, nullptr);
    AssertRelease(FWindow != nullptr);

    glfwSetWindowSizeCallback(FWindow, &WindowSizeCallback);

    glfwSetMouseButtonCallback(FWindow, &WindowMouseButtonCallback);
    glfwSetScrollCallback(FWindow, &WindowScrollCallback);
    glfwSetKeyCallback(FWindow, &WindowKeyCallback);
    glfwSetCharCallback(FWindow, &WindowCharCallback);
    glfwSetCursorPosCallback(FWindow, &WindowMousePosCallback);

    glfwSetJoystickCallback(&WindowJoystickCallback);

    // Init input manager
    double mouse_x, mouse_y;
    glfwGetCursorPos(FWindow, &mouse_x, &mouse_y);

    Input::Initialise(InputKeyNames::INPUT_KEY_LAST, vec2((float)mouse_x, (float)mouse_y), MouseButtons::MOUSE_BUTTON_LAST);

    InitJoysticks();
}

void GLFWDisplayWindowHandler::Shutdown()
{
    Input::Shutdown();
    AlwaysCheckedAssert(FWindow != nullptr);
    if (FWindow != nullptr)
    {
        glfwDestroyWindow(FWindow);
        FWindow = nullptr;
    }
    glfwTerminate();
}

void* GLFWDisplayWindowHandler::GetNativeWindowHandle() const
{
    AssertRelease(FWindow != nullptr);
    return glfwGetWin32Window(FWindow);
}

GLFWwindow* GLFWDisplayWindowHandler::GetWindowHandle() const
{
    AssertRelease(FWindow != nullptr);
    return FWindow;
}

bool GLFWDisplayWindowHandler::ShouldClose()
{
    AssertRelease(FWindow != nullptr);
    return glfwWindowShouldClose(FWindow);
}

void GLFWDisplayWindowHandler::PollEvents()
{
    AssertRelease(FWindow != nullptr);
    glfwPollEvents();
}

void GLFWDisplayWindowHandler::ResizeWindow(GLFWwindow* window, int width, int height)
{
    AssertRelease(FWindow != nullptr);
    AlwaysCheckedAssert(window == FWindow);

    FHeight = height;
    FWidth = width;
}

void GLFWDisplayWindowHandler::InitInputsForImGui(ImGuiIO& io)
{
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "ECSEngine_GLFW";

    // Create mouse cursors
    // (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
    // GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
    // Missing cursors will return NULL and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
    GLFWerrorfun prev_error_callback = glfwSetErrorCallback(NULL);
    FMouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    FMouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    FMouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    FMouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    FMouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if defined(GLFW_RESIZE_ALL_CURSOR)
    FMouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
    FMouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
    FMouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    FMouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
    FMouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    FMouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    FMouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    FMouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
    glfwSetErrorCallback(prev_error_callback);
}

void GLFWDisplayWindowHandler::UpdateMousePosAndButtonsForImGUI(ImGuiIO& io)
{
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
        io.AddMouseButtonEvent(i, glfwGetMouseButton(FWindow, i) != 0);

    if (io.WantSetMousePos)
    {
        glfwSetCursorPos(FWindow, (double)io.MousePos.x, (double)io.MousePos.y);
    }
    else if (glfwGetWindowAttrib(FWindow, GLFW_FOCUSED) != 0)
    {
        double mouse_x, mouse_y;
        glfwGetCursorPos(FWindow, &mouse_x, &mouse_y);
        io.AddMousePosEvent((float)mouse_x, (float)mouse_y);
    }
    else
    {
        io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
    }
}

void GLFWDisplayWindowHandler::UpdateMouseCursorForImGUI(ImGuiIO& io)
{
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(FWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        glfwSetInputMode(FWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else
    {
        // Show OS mouse cursor
        // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
        glfwSetCursor(FWindow, FMouseCursors[imgui_cursor] ? FMouseCursors[imgui_cursor] : FMouseCursors[ImGuiMouseCursor_Arrow]);
        glfwSetInputMode(FWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void GLFWDisplayWindowHandler::UpdateJoysticks(ImGuiIO& io)
{
    UpdateGamepads();
}

float GLFWDisplayWindowHandler::AspectRatio() const
{
    return (FHeight > 0 && FWidth > 0) ? (float)FWidth / (float)FHeight : 1.f;
}

} // namespace Rendering
} // namespace ECSEngine
