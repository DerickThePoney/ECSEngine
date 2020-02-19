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

} // namespace Rendering
} // namespace ECSEngine