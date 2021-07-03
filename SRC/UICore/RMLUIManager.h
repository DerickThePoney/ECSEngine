#pragma once
#include "Common/Singleton.h"

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
    RmlSystemInterface* FSystemInterface = nullptr;
    Rendering::RmlRenderer* FRenderInterface = nullptr;
};
} // namespace UI
} // namespace ECSEngine