#include "stdafx.h"

#include "DisplayWindow.h"
namespace ECSEngine
{
namespace Rendering
{
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

} // namespace Rendering
} // namespace ECSEngine