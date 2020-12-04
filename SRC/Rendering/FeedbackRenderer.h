#pragma once
#include "Common/RenderingHandles.h"
#include "Common/Singleton.h"
#include "RenderingCore/DrawCommands.h"

namespace ECSEngine
{
namespace Rendering
{
class FramebufferInstance;
class FeedbackRenderer : public Singleton<FeedbackRenderer>
{
public:
    FeedbackRenderer();
    ~FeedbackRenderer();

    void Initialise();
    void Shutdown();

    void Render();

private:
    Rendering::DrawCommandBuffer* FDrawCommandBuffer;
    u32 FCameraId;
    FramebufferInstance* FFramebuffer;
};
} // namespace Rendering
} // namespace ECSEngine
