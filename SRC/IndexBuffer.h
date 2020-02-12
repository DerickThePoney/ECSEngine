#pragma once
#include "bgfx/bgfx.h"

namespace ECSEngine
{
namespace Rendering
{
class IndexBuffer final
{
public:
    IndexBuffer();
    ~IndexBuffer();

    u32 GetByteSize() const;
    u32 GetSize() const;

    u32* GetData();
    const u32* GetData() const;

    void SetData(const void* parSrc, u32 parSize);

    const bgfx::IndexBufferHandle& GetIndexBufferHandle();

private:
    void CreateIndexBufferHandle();
    void DestroyIndexBufferHandleIFN();

private:
    u32* FData;
    u32 FSize;

    bool FHandleHasBeenComputed;
    bgfx::IndexBufferHandle FHandle;
};
} // namespace Rendering
} // namespace ECSEngine
