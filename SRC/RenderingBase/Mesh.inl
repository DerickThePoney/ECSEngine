namespace ECSEngine
{
namespace Rendering
{
template<class VertexLayout>
void Mesh<VertexLayout>::SetRawVertexData(const void* parSrc, u32 parSizeInBytes, const bool parComputeHandle /*= true*/)
{
    FVertexBuffer.SetRawData(parSrc, parSizeInBytes);
    if (parComputeHandle)
    {
        FVertexBuffer.CreateVertexBufferHandle();
        AssertRelease(bgfx::isValid(FVertexBuffer.GetVertexBufferHandle()));
    }
}

template<class VertexLayout>
void Mesh<VertexLayout>::SetRawIndexData(const void* parSrc, u32 parSizeInBytes, const bool parComputeHandle /*= true*/)
{
    FIndexBuffer.SetData(parSrc, parSizeInBytes);
    if (parComputeHandle)
    {
        const bgfx::IndexBufferHandle& handle = FIndexBuffer.GetIndexBufferHandle();
        AssertRelease(bgfx::isValid(handle));
    }
}

template<class VertexLayout>
const bgfx::IndexBufferHandle& Mesh<VertexLayout>::GetIndexBufferHandle()
{
    const bgfx::IndexBufferHandle& handle = FIndexBuffer.GetIndexBufferHandle();
    AssertRelease(bgfx::isValid(handle));
    return handle;
}

template<class VertexLayout>
const bgfx::VertexBufferHandle& Mesh<VertexLayout>::GetVertexBufferHandle()
{
    const bgfx::VertexBufferHandle& handle = FVertexBuffer.GetVertexBufferHandle();
    AssertRelease(bgfx::isValid(handle));
    return handle;
}

} // namespace Rendering
} // namespace ECSEngine
