#include "bgfx/bgfx.h"

namespace ECSEngine
{
namespace Rendering
{
template<typename VertexLayout>
VertexBuffer<VertexLayout>::VertexBuffer()
    : FData(nullptr)
    , FSize(0)
    , FHandleHasBeenComputed(false)
    , FHandle()
{
    FHandle.idx = bgfx::kInvalidHandle;
}

template<typename VertexLayout>
VertexBuffer<VertexLayout>::~VertexBuffer()
{
    DestroyHandleIFN();

    if (FData != nullptr)
    {
        delete[] FData;
        FData = nullptr;
    }

    FSize = 0;
}

template<typename VertexLayout>
const bgfx::VertexBufferHandle& VertexBuffer<VertexLayout>::GetVertexBufferHandle()
{
    if (!FHandleHasBeenComputed)
        CreateVertexBufferHandle();

    AssertRelease(bgfx::isValid(FHandle));
    AlwaysCheckedAssert(FHandleHasBeenComputed);
    return FHandle;
}

template<typename VertexLayout>
void VertexBuffer<VertexLayout>::DestroyHandleIFN()
{
    if (FHandleHasBeenComputed)
        bgfx::destroy(FHandle);

    FHandle.idx = bgfx::kInvalidHandle;

    FHandleHasBeenComputed = false;
}

template<typename VertexLayout>
void VertexBuffer<VertexLayout>::SetRawData(const void* parSrc, u32 parSizeInBytes)
{
    DestroyHandleIFN();

    if (FData != nullptr)
    {
        delete[] FData;
        FData = nullptr;
    }

    FData = new VertexLayout[parSizeInBytes / sizeof(VertexLayout)];
    AssertRelease(FData != nullptr);
    memcpy(FData, parSrc, parSizeInBytes);

    FSize = parSizeInBytes / sizeof(VertexLayout);
}

template<typename VertexLayout>
void VertexBuffer<VertexLayout>::CreateVertexBufferHandle()
{
    AlwaysCheckedAssert(!FHandleHasBeenComputed);
    FHandle = bgfx::createVertexBuffer(bgfx::makeRef(GetRawData(), GetByteSize()), GetVertexLayout(VertexLayout()));
    AssertRelease(bgfx::isValid(FHandle));
    FHandleHasBeenComputed = true;
}

} // namespace Rendering
} // namespace ECSEngine
