#pragma once
#include "Common/PoolAllocator.h"

namespace ECSEngine
{
namespace Rendering
{
class MeshHandle;
class MaterialInstanceHandle;

//----------------------------------------------------------------
//          IDrawCommand
//----------------------------------------------------------------
class IDrawCommand
{
public:
    IDrawCommand(const u16 parViewId)
        : FViewId(parViewId)
    {
    }
    virtual ~IDrawCommand() {}
    virtual void SubmitCommand() const = 0;

protected:
    u16 FViewId;
};

//----------------------------------------------------------------
//          DrawCommandBuffer
//----------------------------------------------------------------
class DrawCommandBuffer
{
    DECLARE_POOL_ALLOCATED(DrawCommandBuffer);

public:
    DrawCommandBuffer(const u16 parViewId);
    ~DrawCommandBuffer();

    DrawCommandBuffer(const DrawCommandBuffer& other) = delete;
    DrawCommandBuffer(DrawCommandBuffer&& other) = delete;

    void operator=(const DrawCommandBuffer& other) = delete;
    void operator=(DrawCommandBuffer&& other) = delete;

    void reserve(u32 parSize);
    void clear();

    void SetViewTranform(const glm::mat4& parViewTransform, const glm::mat4& parProjection);
    void DrawMesh(const MeshHandle& parMeshHandle, const MaterialInstanceHandle& parMaterialInstanceHandle, const glm::mat4& parTransform = glm::identity<glm::mat4>());
    void DrawAABB(const MaterialInstanceHandle& parMaterialInstanceHandle, const glm::vec3& parMin, const glm::vec3& parMax, const u32 parColor = 0xFFFFFFFF);
    void DrawAABBAsCube(const MaterialInstanceHandle& parMaterialInstanceHandle, const glm::vec3& parMin, const glm::vec3& parMax, const u32 parColor = 0xFFFFFFFF);
    void DrawFrustum(const MaterialInstanceHandle& parMaterialInstanceHandle,
          const glm::mat4& parWorldViewTransform,
          const glm::mat4& parProjectionMatrix,
          const u32 parColor = 0xFFFFFFFF);

    void Submit();

private:
    std::vector<std::unique_ptr<IDrawCommand>> FCommandVector;

    u16 FViewId;
};

} // namespace Rendering
} // namespace ECSEngine
