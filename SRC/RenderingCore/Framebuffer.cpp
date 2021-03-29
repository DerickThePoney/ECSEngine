#include "stdafx.h"

#include "Framebuffer.h"
namespace ECSEngine
{
namespace Rendering
{

IMPLEMENT_POOL_ALLOCATED(FramebufferInstance);

FramebufferInstance::FramebufferInstance(const FramebufferSizeType::Type parSizeType, const glm::vec2 parSize)
    : FFramebufferSizeType(parSizeType)
    , FSize(parSize)
{
}

FramebufferInstance::~FramebufferInstance()
{
    Destroy();
}

glm::uvec2 FramebufferInstance::Size() const
{
    return FSize;
}

bool FramebufferInstance::ShouldResizeWithScreen() const
{
    return FFramebufferSizeType == FramebufferSizeType::SCREEN;
}

void FramebufferInstance::InitFramebuffer()
{
    FFramebufferHandle = bgfx::createFrameBuffer((u8)FAttachments.size(), FAttachments.data(), true);
    AssertRelease(bgfx::isValid(FFramebufferHandle));
}

void FramebufferInstance::Destroy()
{
    if (!bgfx::isValid(FFramebufferHandle))
        return;
    bgfx::destroy(FFramebufferHandle);
    FFramebufferHandle.idx = bgfx::kInvalidHandle;
    FAttachments.clear();
    FFBAttachementsInfos.clear();
}

void FramebufferInstance::AddAttachement(bool parHasMips,
      u16 parNumLayers,
      bgfx::TextureFormat::Enum parFormat,
      u64 parFlags,
      bgfx::Access::Enum parAccess /*= bgfx::Access::Write*/,
      uint16_t parLayer /*= 0*/,
      uint16_t parMip /*= 0*/)
{
    FBAttachmentInfos infos;
    infos.parHasMips = parHasMips;
    infos.parNumLayers = parNumLayers;
    infos.parFormat = parFormat;
    infos.parFlags = parFlags;
    infos.parAccess = parAccess;
    infos.parLayer = parLayer;
    infos.parMip = parMip;
    FFBAttachementsInfos.push_back(infos);

    bgfx::TextureHandle handle = bgfx::createTexture2D(FSize.x, FSize.y, parHasMips, parNumLayers, parFormat, parFlags);
    AssertRelease(bgfx::isValid(handle));
    u32 idx = (u32)FAttachments.size();
    FAttachments.resize(FAttachments.size() + 1);
    FAttachments[idx].init(handle, parAccess, parLayer, parMip);
}

bool FramebufferInstance::ResizeIFN(const glm::uvec2 parNewSize)
{
    if (!ShouldResizeWithScreen() || parNewSize == FSize)
        return false;

    SetSize(parNewSize);
    std::vector<FBAttachmentInfos> attachementsInfos = FFBAttachementsInfos;
    Destroy();

    foreachitemconst(att, attachementsInfos) { AddAttachement(att.parHasMips, att.parNumLayers, att.parFormat, att.parFlags, att.parAccess, att.parLayer, att.parHasMips); }
    InitFramebuffer();
    return true;
}

const bgfx::TextureHandle FramebufferInstance::GetTextureHandle(u32 parAttachment)
{
    if (parAttachment < FAttachments.size())
    {
        return FAttachments[parAttachment].handle;
    }
    bgfx::TextureHandle res;
    res.idx = bgfx::kInvalidHandle;
    return res;
}

} // namespace Rendering
} // namespace ECSEngine
