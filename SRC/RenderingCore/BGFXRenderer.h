#pragma once
#include "Common/Singleton.h"
#include "RenderPass.h"

namespace ECSEngine
{
using SpecificFrameObserver = Delegate<void()>;
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

    DrawCommandBuffer* CreateCommandBuffer(RenderPassId::Type parViewId = RenderPassId::GEOMETRY_PASS);
    void ReleaseCommandBuffer(DrawCommandBuffer* buffer);

    void AddRequestOnSpecificFrame(const u32 parFrameNumber, const SpecificFrameObserver& parObserver);

    u32 GetCurrentFrame() const { return FCurrentFrame; }

private:
    u32 FCurrentFrame;

    std::map<u32, std::list<SpecificFrameObserver>> FSpecificFrameObserver;
};

} // namespace Rendering
} // namespace ECSEngine