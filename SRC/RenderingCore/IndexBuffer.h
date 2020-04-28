#pragma once

namespace ECSEngine
{
namespace Rendering
{
class IndexBuffer
{
public:
    IndexBuffer();
    ~IndexBuffer();

    u32 GetByteSize() const;
    u32 GetSize() const;

    void SetData(const void* parSrc, u32 parSize);

    const bgfx::IndexBufferHandle& GetIndexBufferHandle();

private:
    void CreateIndexBufferHandle(const void* parSrc);
    void DestroyIndexBufferHandleIFN();

private:
    u32 FSize;
    bool FHandleHasBeenComputed;
    bgfx::IndexBufferHandle FHandle;
};
} // namespace Rendering
} // namespace ECSEngine
