#pragma once
#include "Common/Singleton.h"

struct GLFWwindow;
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

    void* GetNativeWindowHandle() const;

    GLFWwindow* GetWindowHandle() const;

    bool ShouldClose();

    void PollEvents();

    void ResizeWindow(GLFWwindow* window, int width, int height);

    void InitInputsForImGui(ImGuiIO& io);

    void UpdateMousePosAndButtonsForImGUI(ImGuiIO& io);

private:
    std::string FName;
    u32 FWidth;
    u32 FHeight;

    GLFWwindow* FWindow;
};
} // namespace Rendering
} // namespace ECSEngine
