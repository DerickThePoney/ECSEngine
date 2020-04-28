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

} // namespace Rendering
} // namespace ECSEngine