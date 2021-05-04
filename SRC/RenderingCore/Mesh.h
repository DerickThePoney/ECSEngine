#pragma once
#include "IndexBuffer.h"
#include "VertexBuffer.h"

namespace bgfx
{
struct IndexBufferHandle;
struct VertexBufferHandle;
struct DynamicIndexBufferHandle;
struct DynamicVertexBufferHandle;
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
    virtual ~Mesh() { }

    virtual const bgfx::VertexBufferHandle& GetVertexBufferHandle();

    virtual const bgfx::IndexBufferHandle& GetIndexBufferHandle();

    virtual void SetRawVertexData(const VertexDataStream& parVertexData);

    virtual void SetRawIndexData(const void* parSrc, u32 parSizeInBytes);

    virtual void SetBoundingCircle(const glm::vec4 parBoundingCircle);
    virtual const glm::vec4& BoundingCircle() const { return FBoundingCircle; };

    void SetMeshFilename(const std::string& parFilename)
    {
#ifdef ENABLE_SECURITY_CHECKS
        FMeshFilename = parFilename;
#endif
    }

private:
    IndexBuffer FIndexBuffer;
    VertexBuffer FVertexBuffer;

    glm::vec4 FBoundingCircle;

#ifdef ENABLE_SECURITY_CHECKS
    std::string FMeshFilename;
#endif
};

} // namespace Rendering
} // namespace ECSEngine
