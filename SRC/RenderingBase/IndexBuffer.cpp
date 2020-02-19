#include "stdafx.h"

#include "IndexBuffer.h"

namespace ECSEngine
{
namespace Rendering
{

IndexBuffer::IndexBuffer()
    : FData(nullptr)
    , FSize(0)
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

    if (FData != nullptr)
    {
        delete[] FData;
        FData = nullptr;
    }

    FSize = 0;
}

u32 IndexBuffer::GetByteSize() const
{
    return FSize * sizeof(u32);
}

u32 IndexBuffer::GetSize() const
{
    return FSize;
}

u32* IndexBuffer::GetData()
{
    return FData;
}

const u32* IndexBuffer::GetData() const
{
    return FData;
}

void IndexBuffer::SetData(const void* parSrc, u32 parSize)
{
    if (FHandleHasBeenComputed)
        DestroyIndexBufferHandleIFN();

    AssertRelease(!bgfx::isValid(FHandle));
    AssertRelease(!FHandleHasBeenComputed);
    if (FData != nullptr)
    {
        delete[] FData;
        FData = nullptr;
        FSize = 0;
    }

    AssertRelease((parSize % sizeof(u32)) == 0);
    FSize = parSize / sizeof(u32);
    FData = new u32[FSize];
    AssertRelease(FData != nullptr);
    memcpy(FData, parSrc, parSize);
}

const bgfx::IndexBufferHandle& IndexBuffer::GetIndexBufferHandle()
{
    if (!FHandleHasBeenComputed)
    {
        CreateIndexBufferHandle();
    }
    AssertRelease(FHandleHasBeenComputed);
    AssertRelease(bgfx::isValid(FHandle));

    return FHandle;
}

void IndexBuffer::CreateIndexBufferHandle()
{
    AlwaysCheckedAssert(!FHandleHasBeenComputed);
    FHandle = bgfx::createIndexBuffer(bgfx::makeRef(FData, GetByteSize()), BGFX_BUFFER_INDEX32);
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

} // namespace Rendering
} // namespace ECSEngine
