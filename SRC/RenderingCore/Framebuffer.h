#pragma once
#include "Common/PoolAllocator.h"

namespace ECSEngine
{
namespace Rendering
{
namespace FramebufferType
{
enum Type : u8
{
    CUSTOM,
    SCREEN
};
}

namespace FramebufferSizeType
{
enum Type : u8
{
    SCREEN,
    CUSTOM
};
}

class FramebufferInstance
{
    DECLARE_POOL_ALLOCATED(FramebufferInstance);

public:
    FramebufferInstance(const FramebufferSizeType::Type parSizeType, const uvec2 parSize);
    ~FramebufferInstance();

    void InitFramebuffer();
    void Destroy();

    void SetSize(const uvec2 parNewSize) { FSize = parNewSize; }
    uvec2 Size() const;
    bool ShouldResizeWithScreen() const;

    void AddAttachement(bool parHasMips,
          u16 parNumLayers,
          bgfx::TextureFormat::Enum parFormat,
          u64 parFlags,
          bgfx::Access::Enum parAccess = bgfx::Access::Write,
          u16 parLayer = 0,
          u16 parMip = 0);

    bool ResizeIFN(const uvec2 parNewSize);

    const bgfx::TextureHandle GetTextureHandle(u32 parAttachment);
    const bgfx::FrameBufferHandle GetHandle() const { return FFramebufferHandle; }

private:
    FramebufferSizeType::Type FFramebufferSizeType;
    std::vector<bgfx::Attachment> FAttachments;

    struct FBAttachmentInfos
    {
        bool parHasMips = false;
        u16 parNumLayers = 1;
        bgfx::TextureFormat::Enum parFormat = bgfx::TextureFormat::RGB8;
        u64 parFlags = 0;
        bgfx::Access::Enum parAccess = bgfx::Access::Write;
        u16 parLayer = 0;
        u16 parMip = 0;
    };

    std::vector<FBAttachmentInfos> FFBAttachementsInfos;

    bgfx::FrameBufferHandle FFramebufferHandle;
    uvec2 FSize;
};
} // namespace Rendering
} // namespace ECSEngine
