#pragma once

namespace ECSEngine
{
namespace Rendering
{
class FramebufferInstance;
class DrawCommandBuffer;

class GameRenderer
{
public:
    void Initialise();
    void Shutdown();

    void Render();

private:
    u32 FGameplayCameraId = -1;

    FramebufferInstance* FGeometryFramebuffer = nullptr;
    FramebufferInstance* FFeedbackFramebuffer = nullptr;

    DrawCommandBuffer* FGeometryCommandBuffer = nullptr;
    DrawCommandBuffer* FFeedbackCommandBuffer = nullptr;
    DrawCommandBuffer* FCombineCommandBuffer = nullptr;
};
} // namespace Rendering
} // namespace ECSEngine