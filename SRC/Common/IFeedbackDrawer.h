#pragma once

namespace ECSEngine
{
namespace Rendering
{
class DrawCommandBuffer;
}
class IFeedbackDrawer
{
public:
    void DrawFeedback(Rendering::DrawCommandBuffer* parCommandBuffer);

protected:
    virtual void VirtualDrawFeedback(Rendering::DrawCommandBuffer* parCommandBuffer) = 0;
};
} // namespace ECSEngine
