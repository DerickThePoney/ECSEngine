#include "stdafx.h"

#include "DisplayWindow.h"
//clang-format off
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
//clang-format on

#include "BGFXRenderer.h"

namespace ECSEngine
{
namespace Rendering
{

namespace
{
void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    AssertRelease(DisplayWindow::HasInstance());
    DisplayWindow::Instance().ResizeWindow(window, width, height);
    AssertRelease(BGFXRenderer::HasInstance());
    BGFXRenderer::Instance().Resize(width, height);
}

void WindowScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}

void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
#ifdef _WIN32
    io.KeySuper = false;
#else
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
}

void WindowCharCallback(GLFWwindow* window, unsigned int c)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(c);
}
} // namespace
DisplayWindow::DisplayWindow()
    : FWindow(nullptr)
    , FWidth(800)
    , FHeight(600)
    , FName("DEFAULT_NAME_CHANGE_IT_OR DIE!!!")
{
}

DisplayWindow::~DisplayWindow()
{
    AlwaysCheckedAssert(FWindow == nullptr);
    if (FWindow != nullptr)
        Destroy();
}

void DisplayWindow::Init()
{
    glfwInit();

    AlwaysCheckedAssert(FWindow == nullptr);
    if (FWindow != nullptr)
        Destroy();

    FWindow = glfwCreateWindow(FWidth, FHeight, FName.c_str(), nullptr, nullptr);
    AssertRelease(FWindow != nullptr);

    glfwSetWindowSizeCallback(FWindow, &WindowSizeCallback);

    /*glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);*/
    glfwSetScrollCallback(FWindow, &WindowScrollCallback);
    glfwSetKeyCallback(FWindow, &WindowKeyCallback);
    glfwSetCharCallback(FWindow, &WindowCharCallback);
}

void DisplayWindow::Shutdown()
{
    AlwaysCheckedAssert(FWindow != nullptr);
    if (FWindow != nullptr)
    {
        glfwDestroyWindow(FWindow);
        FWindow = nullptr;
    }
    glfwTerminate();
}

void* DisplayWindow::GetNativeWindowHandle() const
{
    AssertRelease(FWindow != nullptr);
    return glfwGetWin32Window(FWindow);
}

GLFWwindow* DisplayWindow::GetWindowHandle() const
{
    AssertRelease(FWindow != nullptr);
    return FWindow;
}

bool DisplayWindow::ShouldClose()
{
    AssertRelease(FWindow != nullptr);
    return glfwWindowShouldClose(FWindow);
}

void DisplayWindow::PollEvents()
{
    AssertRelease(FWindow != nullptr);
    glfwPollEvents();
}

void DisplayWindow::ResizeWindow(GLFWwindow* window, int width, int height)
{
    AssertRelease(FWindow != nullptr);
    AlwaysCheckedAssert(window == FWindow);

    FHeight = height;
    FWidth = width;
}

void DisplayWindow::InitInputsForImGui(ImGuiIO& io)
{
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_glfw";

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
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
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    // TODO IMGUI
    /*io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
    io.ClipboardUserData = g_Window;*/
#if defined(_WIN32)
    io.ImeWindowHandle = GetNativeWindowHandle();
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

void DisplayWindow::UpdateMousePosAndButtonsForImGUI(ImGuiIO& io)
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

void DisplayWindow::UpdateMouseCursorForImGUI(ImGuiIO& io)
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

} // namespace Rendering
} // namespace ECSEngine