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
class IMesh
{
public:
    IMesh() {}
    virtual ~IMesh() {}
    virtual const bgfx::VertexBufferHandle& GetVertexBufferHandle() = 0;
    virtual const bgfx::IndexBufferHandle& GetIndexBufferHandle() = 0;

    virtual void SetRawVertexData(const void* parSrc, u32 parSizeInBytes, const bool parComputeHandle = true) = 0;
    virtual void SetRawIndexData(const void* parSrc, u32 parSizeInBytes, const bool parComputeHandle = true) = 0;
};

template<class VertexLayout>
class Mesh : public IMesh
{
public:
    Mesh()
        : IMesh()
    {
    }
    virtual ~Mesh() {}

    virtual const bgfx::VertexBufferHandle& GetVertexBufferHandle() override;

    virtual const bgfx::IndexBufferHandle& GetIndexBufferHandle() override;

    virtual void SetRawVertexData(const void* parSrc, u32 parSizeInBytes, const bool parComputeHandle = true) override;

    virtual void SetRawIndexData(const void* parSrc, u32 parSizeInBytes, const bool parComputeHandle = true) override;

private:
    VertexBuffer<VertexLayout> FVertexBuffer;
    IndexBuffer FIndexBuffer;
};

} // namespace Rendering
} // namespace ECSEngine

#include "Mesh.inl"