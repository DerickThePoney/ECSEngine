#include "stdafx.h"

#include "Mesh.h"
namespace ECSEngine
{
namespace Rendering
{

void Mesh::SetRawVertexData(const VertexDataStream& parVertexData)
{
    FVertexBuffer.SetRawData(parVertexData);

    AssertRelease(bgfx::isValid(FVertexBuffer.GetVertexBufferHandle()));
}

void Mesh::SetRawIndexData(const void* parSrc, u32 parSizeInBytes)
{
    FIndexBuffer.SetData(parSrc, parSizeInBytes);

    AssertRelease(bgfx::isValid(FIndexBuffer.GetIndexBufferHandle()));
}

void Mesh::SetBoundingCircle(const glm::vec4 parBoundingCircle)
{
    FBoundingCircle = parBoundingCircle;
}

const bgfx::IndexBufferHandle& Mesh::GetIndexBufferHandle()
{
    const bgfx::IndexBufferHandle& handle = FIndexBuffer.GetIndexBufferHandle();
    AssertRelease(bgfx::isValid(handle));
    return handle;
}

const bgfx::VertexBufferHandle& Mesh::GetVertexBufferHandle()
{
    const bgfx::VertexBufferHandle& handle = FVertexBuffer.GetVertexBufferHandle();
    AssertRelease(bgfx::isValid(handle));
    return handle;
}
} // namespace Rendering
} // namespace ECSEngine