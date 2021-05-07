#pragma once
#include "Common/Singleton.h"

struct GLFWwindow;
struct GLFWcursor;
struct ImGuiIO;

namespace ECSEngine
{
namespace Rendering
{

class GLFWDisplayWindowHandler : public Singleton<GLFWDisplayWindowHandler>
{
public:
    GLFWDisplayWindowHandler();
    ~GLFWDisplayWindowHandler();

    void Init();
    void Shutdown();

    const std::string& GetName() const { return FName; }
    void SetName(const std::string& parName) { FName = parName; }

    float AspectRatio() const { return (float)FWidth / (float)FHeight; }
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
    void UpdateMouseCursorForImGUI(ImGuiIO& io);
    void UpdateJoysticks(ImGuiIO& io);

private:
    std::string FName;
    u32 FWidth;
    u32 FHeight;

    GLFWwindow* FWindow;
    GLFWcursor** FMouseCursors = nullptr;
};
} // namespace Rendering
} // namespace ECSEngine
