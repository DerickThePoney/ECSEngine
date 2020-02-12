#pragma once
#include "bgfx/bgfx.h"

namespace ECSEngine
{
namespace Rendering
{
template<typename VertexLayout>
class VertexBuffer final
{
public:
    VertexBuffer();
    ~VertexBuffer();

    u32 GetNumberOfVertices() const { return FSize; }

    u32 GetByteSize() const { return FSize * sizeof(VertexLayout); }

    const void* GetRawData() const { return (void*)FData; }

    void* GetRawData() { return (void*)FData; }

    void SetRawData(const void* parSrc, u32 parSizeInBytes);

    void CreateVertexBufferHandle();

    void DestroyHandleIFN();
    const bgfx::VertexBufferHandle& GetVertexBufferHandle();

    template<typename T>
    const T* GetDataAs() const
    {
        return reinterpret_cast<T*>(GetRawData());
    }

    template<typename T>
    T* GetDataAs()
    {
        return reinterpret_cast<T*>(GetRawData());
    }

private:
    VertexLayout* FData;
    u32 FSize;

    bool FHandleHasBeenComputed;
    bgfx::VertexBufferHandle FHandle;
};

} // namespace Rendering
} // namespace ECSEngine

#include "VertexBuffer.inl"