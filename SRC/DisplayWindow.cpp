#include "stdafx.h"

#include "DisplayWindow.h"

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