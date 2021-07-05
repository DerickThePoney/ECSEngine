#pragma once
#include "Common/Singleton.h"

namespace Rml
{
class Context;
class ElementDocument;
} // namespace Rml

namespace ECSEngine
{
namespace Rendering
{
class RmlRenderer;
}
namespace UI
{
class RmlSystemInterface;
class RmlFileInterface;
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

    void LoadDocument();

private:
    Rml::Context* FContext = nullptr;
    RmlSystemInterface* FSystemInterface = nullptr;
    Rendering::RmlRenderer* FRenderInterface = nullptr;
    RmlFileInterface* FFileInterface = nullptr;
    Rml::ElementDocument* doc = nullptr;
};
} // namespace UI
} // namespace ECSEngine