#include "stdafx.h"

#include "VertexBuffer.h"

#include "VertexLayout.h"

namespace ECSEngine
{
namespace Rendering
{

VertexBuffer::VertexBuffer()
    : FHandleHasBeenComputed(false)
    , FHandle()
    , FSize(0)
    , FByteSize(0)
{
    FHandle.idx = bgfx::kInvalidHandle;
}

VertexBuffer::~VertexBuffer()
{
    DestroyHandleIFN();
}

const bgfx::VertexBufferHandle& VertexBuffer::GetVertexBufferHandle()
{
    AlwaysCheckedAssert(FHandleHasBeenComputed);
    AssertRelease(bgfx::isValid(FHandle));
    return FHandle;
}

void VertexBuffer::DestroyHandleIFN()
{
    if (FHandleHasBeenComputed)
        bgfx::destroy(FHandle);

    FHandle.idx = bgfx::kInvalidHandle;

    FHandleHasBeenComputed = false;
}

void VertexBuffer::SetRawData(const VertexDataStream& parDataStream)
{
    DestroyHandleIFN();

    FSize = parDataStream.GetSize();
    FByteSize = parDataStream.GetByteSize();

    CreateVertexBufferHandle(parDataStream);
}

void VertexBuffer::CreateVertexBufferHandle(const VertexDataStream& parDataStream)
{
    AlwaysCheckedAssert(!FHandleHasBeenComputed);
    FHandle = bgfx::createVertexBuffer(bgfx::copy(parDataStream.GetData(), GetByteSize()), GetVertexLayout(parDataStream.GetHash()));
    AssertRelease(bgfx::isValid(FHandle));
    FHandleHasBeenComputed = true;
}

DynamicVertexBuffer::DynamicVertexBuffer(VertexLayoutHash parHash)
    : FHash(parHash)
{
    FHandle.idx = bgfx::kInvalidHandle;
    FLayoutHandle.idx = bgfx::kInvalidHandle;
}

DynamicVertexBuffer::DynamicVertexBuffer()
{
    FHandle.idx = bgfx::kInvalidHandle;
    FLayoutHandle.idx = bgfx::kInvalidHandle;
}

DynamicVertexBuffer::~DynamicVertexBuffer()
{
    DestroyHandleIFN();
}

void DynamicVertexBuffer::PushRawBuffer(const VertexDataStream& parDataStream)
{
    AssertRelease(FHash == parDataStream.GetHash());
    if (!FHandleHasBeenComputed)
    {
        CreateVertexBufferHandle(parDataStream);
        FSize = parDataStream.GetSize();
        FByteSize = parDataStream.GetByteSize();
    }
    else
    {
        AssertRelease(bgfx::isValid(FHandle));
        bgfx::update(FHandle, 0, bgfx::copy(parDataStream.GetData(), parDataStream.GetByteSize()));
        FSize = parDataStream.GetSize();
        FByteSize = parDataStream.GetByteSize();
    }
    FAllocatedSize = std::max(FSize, FAllocatedSize);
}

void DynamicVertexBuffer::DestroyHandleIFN()
{
    if (FHandleHasBeenComputed)
    {
        bgfx::destroy(FHandle);
        bgfx::destroy(FLayoutHandle);
    }

    FHandle.idx = bgfx::kInvalidHandle;
    FLayoutHandle.idx = bgfx::kInvalidHandle;
    FHandleHasBeenComputed = false;
}

const bgfx::DynamicVertexBufferHandle& DynamicVertexBuffer::GetVertexBufferHandle() const
{
    AlwaysCheckedAssert(FHandleHasBeenComputed);
    AssertRelease(bgfx::isValid(FHandle));
    return FHandle;
}

void DynamicVertexBuffer::CreateVertexBufferHandle(const VertexDataStream& parDataStream)
{
    AlwaysCheckedAssert(!FHandleHasBeenComputed);
    bgfx::VertexLayout layout = GetVertexLayout(FHash);
    FLayoutHandle = bgfx::createVertexLayout(layout);
    AssertRelease(bgfx::isValid(FLayoutHandle));
    FHandle = bgfx::createDynamicVertexBuffer(bgfx::copy(parDataStream.GetData(), parDataStream.GetByteSize()), layout, BGFX_BUFFER_ALLOW_RESIZE);
    AssertRelease(bgfx::isValid(FHandle));
    FHandleHasBeenComputed = true;
}

const bgfx::VertexLayoutHandle& DynamicVertexBuffer::GetVertexLayoutHandle() const
{
    AlwaysCheckedAssert(FHandleHasBeenComputed);
    AssertRelease(bgfx::isValid(FLayoutHandle));
    return FLayoutHandle;
}

} // namespace Rendering
} // namespace ECSEngine
