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

} // namespace Rendering
} // namespace ECSEngine
