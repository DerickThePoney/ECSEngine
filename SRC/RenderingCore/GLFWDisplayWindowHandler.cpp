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
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheelH += (float)xoffset;
        io.MouseWheel += (float)yoffset;
    }

    Input::SetMouseScrollDelta(vec2((float)xoffset, (float)yoffset));
}

void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    bool ctrl = false, shift = false, alt = false;
    for (u16 i = (u16)RenderPassId::IMGUI_PASSES_START; i < (u16)RenderPassId::IMGUI_PASSES_END + 1; i++)
    {
        ImGUI::SetImGuiContext((RenderPassId::Type)i);
        ImGuiIO& io = ImGui::GetIO();
        if (action == GLFW_PRESS)
            io.KeysData[key].Down = true;
        if (action == GLFW_RELEASE)
            io.KeysData[key].Down = false;

        // Modifiers are not reliable across systems
        io.KeyCtrl = io.KeyCtrl;
        io.KeyShift = io.KeyShift;
        io.KeyAlt = io.KeyAlt;
#ifdef _WIN32
        io.KeySuper = false;
#else
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
        ctrl = ctrl || io.KeyCtrl;
        shift = shift || io.KeyShift;
        alt = alt || io.KeyAlt;
    }

    Input::SetKeyboardButtonState(key, action == GLFW_PRESS || action == GLFW_REPEAT, shift, ctrl, alt);
}

void WindowCharCallback(GLFWwindow* window, unsigned int c)
{
    for (u16 i = (u16)RenderPassId::IMGUI_PASSES_START; i < (u16)RenderPassId::IMGUI_PASSES_END + 1; i++)
    {
        ImGUI::SetImGuiContext((RenderPassId::Type)i);
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(c);
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

    // TODO
    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    /*io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;*/

    // TODO IMGUI
    /*io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
    io.ClipboardUserData = g_Window;*/
#if defined(_WIN32)
    // TODO
    //  ImGuiViewport::PlatformHandleRaw = GetNativeWindowHandle();
#endif* /

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
#if GLFW_HAS_NEW_CURSORS
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
}

void GLFWDisplayWindowHandler::UpdateMousePosAndButtonsForImGUI(ImGuiIO& io)
{
    // Update buttons
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
        io.MouseDown[i] = /*g_MouseJustPressed[i] ||*/ glfwGetMouseButton(FWindow, i) != 0;
        /*g_MouseJustPressed[i] = false;*/
    }

    // Update mouse position
    const ImVec2 mouse_pos_backup = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

    const bool focused = glfwGetWindowAttrib(FWindow, GLFW_FOCUSED) != 0;

    if (focused)
    {
        if (io.WantSetMousePos)
        {
            glfwSetCursorPos(FWindow, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
        }
        else
        {
            double mouse_x, mouse_y;
            glfwGetCursorPos(FWindow, &mouse_x, &mouse_y);
            io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
        }
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
