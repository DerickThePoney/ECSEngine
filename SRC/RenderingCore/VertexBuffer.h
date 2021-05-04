#pragma once
#include "Common/MeshStreamingData.h"

namespace ECSEngine
{
namespace Rendering
{
class VertexBuffer final
{
public:
    VertexBuffer();
    ~VertexBuffer();

    u32 GetNumberOfVertices() const { return FSize; }

    u32 GetByteSize() const { return FByteSize; }

    void SetRawData(const VertexDataStream& parDataStream);

    void DestroyHandleIFN();
    const bgfx::VertexBufferHandle& GetVertexBufferHandle();

private:
    void CreateVertexBufferHandle(const VertexDataStream& parDataStream);

private:
    u32 FSize;
    u32 FByteSize;
    bool FHandleHasBeenComputed;
    bgfx::VertexBufferHandle FHandle;
};

class DynamicVertexBuffer final
{
public:
    DynamicVertexBuffer(VertexLayoutHash parHash);
    ~DynamicVertexBuffer();

    const VertexLayoutHash Hash() const { return FHash; };
    u32 GetNumberOfVertices() const { return FSize; }
    u32 GetByteSize() const { return FByteSize; }

    void PushRawBuffer(const VertexDataStream& parDataStream);

    void DestroyHandleIFN();
    const bgfx::DynamicVertexBufferHandle& GetVertexBufferHandle() const;
    const bgfx::VertexLayoutHandle& GetVertexLayoutHandle() const;

private:
    void CreateVertexBufferHandle(const VertexDataStream& parDataStream);

private:
    VertexLayoutHash FHash;
    u32 FSize = 0;
    u32 FByteSize = 0;
    u32 FAllocatedSize = 0;
    bool FHandleHasBeenComputed = 0;
    bgfx::DynamicVertexBufferHandle FHandle;
    bgfx::VertexLayoutHandle FLayoutHandle;
};

} // namespace Rendering
} // namespace ECSEngine
