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

class DynamicIndexBuffer
{
public:
    DynamicIndexBuffer();
    ~DynamicIndexBuffer();

    u32 GetByteSize() const;
    u32 GetSize() const;

    void PushData(const void* parSrc, u32 parSize);

    const bgfx::DynamicIndexBufferHandle& GetIndexBufferHandle();

private:
    void CreateIndexBufferHandle(const void* parSrc, u32 parByteSize);
    void DestroyIndexBufferHandleIFN();

private:
    u32 FAllocatedSize = 0;
    u32 FSize = 0;
    bool FHandleHasBeenComputed = false;
    bgfx::DynamicIndexBufferHandle FHandle;
};
} // namespace Rendering
} // namespace ECSEngine
