#pragma once
#include "Common/RenderingHandles.h"

namespace ECSEngine
{
namespace Rendering
{
class UICommandBuffer;
} // namespace Rendering
namespace UI
{
class UIBackgroundDrawer
{
public:
    void Draw(Rendering::UICommandBuffer& parBuffer, const glm::vec2 parPosition, const glm::vec2 parSize);

    Rendering::MaterialInstanceHandle FMaterialInstanceHandle;
    u32 FColor = 0xFFFFFFFF;
};
} // namespace UI
} // namespace ECSEngine