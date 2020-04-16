#pragma once
#include "Common/Singleton.h"

namespace ECSEngine
{
namespace Rendering
{
class DrawCommandBuffer;
class BGFXRenderer final : public Singleton<BGFXRenderer>
{
public:
    BGFXRenderer();
    ~BGFXRenderer();

    void Init();
    void RenderFrame();
    void Shutdown();

    void Resize(u32 width, u32 height);

    bool IsInstancingEnabled();

    DrawCommandBuffer& CreateCommandBuffer(u16 parViewId = 0);

private:
    std::map<u16, std::vector<DrawCommandBuffer*>> FCommandBuffers;
};

} // namespace Rendering
} // namespace ECSEngine