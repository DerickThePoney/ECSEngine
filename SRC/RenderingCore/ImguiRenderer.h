#pragma once
#include "Common/Singleton.h"
#include "RenderPass.h"

namespace ECSEngine
{
namespace Rendering
{

#define IMGUI_FLAGS_NONE UINT8_C(0x00)
#define IMGUI_FLAGS_ALPHA_BLEND UINT8_C(0x01)

namespace ImGUI
{
void Init();
void SetImGuiContext(RenderPassId::Type parImGuiPass);
void NewFrame();
void Render();
void Shutdown();
} // namespace ImGUI
} // namespace Rendering
} // namespace ECSEngine
