#pragma once
#include "Common/Singleton.h"

namespace Rml
{
class Context;
}

namespace ECSEngine
{
namespace Rendering
{
class RmlRenderer;
}
namespace UI
{
class RmlSystemInterface;
class RmlUiManager : public Singleton<RmlUiManager>
{
public:
    void Initialise();
    void NewFrame();
    void Update();
    void Render();
    void Shutdown();

private:
    void ProcessInput() const;
    bool ProcessMouse() const;
    bool SetMousePositionHasChanged(glm::vec2 parPos) const;
    bool SetMouseButtons() const;
    bool SetKeyboardButtons() const;
    void SetTextInput() const;

private:
    Rml::Context* FContext = nullptr;
    RmlSystemInterface* FSystemInterface = nullptr;
    Rendering::RmlRenderer* FRenderInterface = nullptr;
};
} // namespace UI
} // namespace ECSEngine