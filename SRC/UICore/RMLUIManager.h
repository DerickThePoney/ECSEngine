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
    void Update();
    void Shutdown();

private:
    void ProcessInput() const;
    void SetMousePositionHasChanged(glm::vec2 parPos) const;

private:
    Rml::Context* FContext = nullptr;
    RmlSystemInterface* FSystemInterface = nullptr;
    Rendering::RmlRenderer* FRenderInterface = nullptr;
};
} // namespace UI
} // namespace ECSEngine