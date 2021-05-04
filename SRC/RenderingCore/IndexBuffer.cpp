#include "stdafx.h"

#include "IndexBuffer.h"

namespace ECSEngine
{
namespace Rendering
{

IndexBuffer::IndexBuffer()
    : FSize(0)
    , FHandleHasBeenComputed(false)
    , FHandle()
{
    FHandle.idx = bgfx::kInvalidHandle;
}

IndexBuffer::~IndexBuffer()
{
    if (FHandleHasBeenComputed)
        DestroyIndexBufferHandleIFN();

    AssertRelease(!bgfx::isValid(FHandle));
    AssertRelease(!FHandleHasBeenComputed);
}

u32 IndexBuffer::GetByteSize() const
{
    return FSize * sizeof(u32);
}

u32 IndexBuffer::GetSize() const
{
    return FSize;
}

void IndexBuffer::SetData(const void* parSrc, u32 parSize)
{
    if (FHandleHasBeenComputed)
        DestroyIndexBufferHandleIFN();

    AssertRelease(!bgfx::isValid(FHandle));
    AssertRelease(!FHandleHasBeenComputed);

    AssertRelease((parSize % sizeof(u32)) == 0);
    FSize = parSize / sizeof(u32);

    CreateIndexBufferHandle(parSrc);
}

const bgfx::IndexBufferHandle& IndexBuffer::GetIndexBufferHandle()
{
    AssertRelease(FHandleHasBeenComputed);
    AssertRelease(bgfx::isValid(FHandle));

    return FHandle;
}

void IndexBuffer::CreateIndexBufferHandle(const void* parSrc)
{
    AlwaysCheckedAssert(!FHandleHasBeenComputed);
    FHandle = bgfx::createIndexBuffer(bgfx::copy(parSrc, GetByteSize()), BGFX_BUFFER_INDEX32);
    AlwaysCheckedAssert(bgfx::isValid(FHandle));
    FHandleHasBeenComputed = true;
}

void IndexBuffer::DestroyIndexBufferHandleIFN()
{
    if (FHandleHasBeenComputed)
    {
        bgfx::destroy(FHandle);

        FHandle.idx = bgfx::kInvalidHandle;
        FHandleHasBeenComputed = false;
    }
}

DynamicIndexBuffer::DynamicIndexBuffer()
{
    FHandle.idx = bgfx::kInvalidHandle;
}

DynamicIndexBuffer::~DynamicIndexBuffer()
{
    if (FHandleHasBeenComputed)
        DestroyIndexBufferHandleIFN();

    AssertRelease(!bgfx::isValid(FHandle));
    AssertRelease(!FHandleHasBeenComputed);
}

u32 DynamicIndexBuffer::GetByteSize() const
{
    return FSize * sizeof(u32);
}

u32 DynamicIndexBuffer::GetSize() const
{
    return FSize;
}

void DynamicIndexBuffer::PushData(const void* parSrc, u32 parSize)
{
    if (!FHandleHasBeenComputed)
    {
        FSize = parSize;
        CreateIndexBufferHandle(parSrc, GetByteSize());
    }
    else
    {
        bgfx::update(FHandle, 0, bgfx::copy(parSrc, parSize * sizeof(u32)));
        FSize = parSize;
    }

    FAllocatedSize = std::max(FSize, FAllocatedSize);
}

const bgfx::DynamicIndexBufferHandle& DynamicIndexBuffer::GetIndexBufferHandle()
{
    AssertRelease(FHandleHasBeenComputed);
    AssertRelease(bgfx::isValid(FHandle));

    return FHandle;
}

void DynamicIndexBuffer::CreateIndexBufferHandle(const void* parSrc, u32 parByteSize)
{
    AlwaysCheckedAssert(!FHandleHasBeenComputed);
    FHandle = bgfx::createDynamicIndexBuffer(bgfx::copy(parSrc, parByteSize), BGFX_BUFFER_ALLOW_RESIZE | BGFX_BUFFER_INDEX32);
    AlwaysCheckedAssert(bgfx::isValid(FHandle));
    FHandleHasBeenComputed = true;
}

void DynamicIndexBuffer::DestroyIndexBufferHandleIFN()
{
    if (FHandleHasBeenComputed)
    {
        bgfx::destroy(FHandle);

        FHandle.idx = bgfx::kInvalidHandle;
        FHandleHasBeenComputed = false;
    }
}

} // namespace Rendering
} // namespace ECSEngine
