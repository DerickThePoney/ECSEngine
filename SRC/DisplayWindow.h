#pragma once
#include "Singleton.h"

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace ECSEngine
{
namespace Rendering
{

class DisplayWindow : public Singleton<DisplayWindow>
{
public:
    DisplayWindow();
    ~DisplayWindow();

    void Init();
    void Shutdown();

    const std::string& GetName() const { return FName; }
    void SetName(const std::string& parName) { FName = parName; }

    glm::uvec2 GetSize() const { return glm::uvec2(FWidth, FHeight); }
    void SetSize(const glm::uvec2& parSize)
    {
        FWidth = parSize.x;
        FHeight = parSize.y;
    }

    void* GetNativeWindowHandle() const
    {
        AssertRelease(FWindow != nullptr);
        return glfwGetWin32Window(FWindow);
    }

    GLFWwindow* GetWindowHandle() const
    {
        AssertRelease(FWindow != nullptr);
        return FWindow;
    }

private:
    std::string FName;
    u32 FWidth;
    u32 FHeight;

    GLFWwindow* FWindow;
};
} // namespace Rendering
} // namespace ECSEngine
