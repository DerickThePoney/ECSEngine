#pragma once
#include "IndexBuffer.h"
#include "VertexBuffer.h"

namespace bgfx
{
struct IndexBufferHandle;
struct VertexBufferHandle;
} // namespace bgfx

namespace ECSEngine
{
namespace Rendering
{
class Mesh
{
public:
    Mesh()
        : FVertexBuffer()
        , FIndexBuffer()
    {
    }
    virtual ~Mesh() {}

    virtual const bgfx::VertexBufferHandle& GetVertexBufferHandle();

    virtual const bgfx::IndexBufferHandle& GetIndexBufferHandle();

    virtual void SetRawVertexData(const VertexDataStream& parVertexData);

    virtual void SetRawIndexData(const void* parSrc, u32 parSizeInBytes);

private:
    IndexBuffer FIndexBuffer;
    VertexBuffer FVertexBuffer;
};

} // namespace Rendering
} // namespace ECSEngine
