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
    FramebufferInstance* FGeometryFramebuffer = nullptr;
    FramebufferInstance* FFeedbackFramebuffer = nullptr;

    DrawCommandBuffer* FGeometryCommandBuffer = nullptr;
    DrawCommandBuffer* FFeedbackCommandBuffer = nullptr;
};
} // namespace Rendering
} // namespace ECSEngine