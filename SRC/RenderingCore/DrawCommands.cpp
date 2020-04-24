#include "stdafx.h"

#include "DrawCommands.h"

#include "Common/Camera.h"
#include "Material.h"
#include "MaterialManager.h"
#include "Mesh.h"
#include "MeshManager.h"
#include "MeshUtils.h"
#include "RenderingState.h"
#include "VertexLayout.h"

#include <bx/bx.h>

namespace ECSEngine
{
namespace Rendering
{
//----------------------------------------------------------------
//          SetViewTransformCommand
//----------------------------------------------------------------
class SetViewTranformCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(SetViewTranformCommand);

public:
    SetViewTranformCommand(const u16 parViewId, const glm::mat4& parViewTransform, const glm::mat4& parProjection);
    virtual ~SetViewTranformCommand();

    virtual void SubmitCommand() const override;

private:
    glm::mat4 FViewTransform;
    glm::mat4 FProjection;
};

IMPLEMENT_POOL_ALLOCATED(SetViewTranformCommand);
SetViewTranformCommand::SetViewTranformCommand(const u16 parViewId, const glm::mat4& parViewTransform, const glm::mat4& parProjection)
    : IDrawCommand(parViewId)
    , FViewTransform(parViewTransform)
    , FProjection(parProjection)
{
}

SetViewTranformCommand::~SetViewTranformCommand()
{
}

void SetViewTranformCommand::SubmitCommand() const
{
    bgfx::setViewTransform(FViewId, &FViewTransform[0][0], &FProjection[0][0]);
}

//----------------------------------------------------------------
//          DrawAABBCommand
//----------------------------------------------------------------
class DrawAABBCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(DrawAABBCommand);

public:
    DrawAABBCommand(const u16 parViewId,
          const MaterialInstanceHandle& parMaterialInstanceHandle,
          const glm::vec3& parMin,
          const glm::vec3& parMax,
          const u32 parColor = 0xFFFFFFFF);
    virtual ~DrawAABBCommand();

    virtual void SubmitCommand() const override;

private:
    glm::vec3 FMin;
    glm::vec3 FMax;
    u32 FColor;
    const MaterialInstanceHandle& FMaterialInstanceHandle;
};

DrawAABBCommand::DrawAABBCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::vec3& parMin,
      const glm::vec3& parMax,
      const u32 parColor /*= 0xFFFFFFFF*/)
    : IDrawCommand(parViewId)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
    , FColor(parColor)
    , FMin(parMin)
    , FMax(parMax)
{
}

DrawAABBCommand::~DrawAABBCommand()
{
}

void DrawAABBCommand::SubmitCommand() const
{
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;

    const bgfx::VertexLayout layout = VertexPositionColorN<1>().GetVertexLayout();
    u32 availableVertices = bgfx::getAvailTransientVertexBuffer(8, layout);
    AssertRelease(availableVertices == 8);
    u32 availableIndices = bgfx::getAvailTransientIndexBuffer(24);
    AssertRelease(availableIndices == 24);

    bgfx::allocTransientBuffers(&vertexBuffer, layout, 8, &indexBuffer, 24);

    VertexPositionColorN<1> vertices[8];
    forrange(i, 0, 8) { vertices[i].FColor[0] = FColor; }

    vertices[0].FPosition = FMin;
    vertices[1].FPosition = glm::vec3(FMax.x, FMin.y, FMin.z);
    vertices[2].FPosition = glm::vec3(FMax.x, FMin.y, FMax.z);
    vertices[3].FPosition = glm::vec3(FMin.x, FMin.y, FMax.z);

    vertices[4].FPosition = glm::vec3(FMin.x, FMax.y, FMin.z);
    vertices[5].FPosition = glm::vec3(FMax.x, FMax.y, FMin.z);
    vertices[6].FPosition = FMax;
    vertices[7].FPosition = glm::vec3(FMin.x, FMax.y, FMax.z);

    bx::memCopy(vertexBuffer.data, vertices, sizeof(vertices));

    u16 indices[24];

    u32 idx = 0;

    indices[idx++] = 0;
    indices[idx++] = 1;
    indices[idx++] = 1;
    indices[idx++] = 2;
    indices[idx++] = 2;
    indices[idx++] = 3;
    indices[idx++] = 3;
    indices[idx++] = 0;

    indices[idx++] = 4;
    indices[idx++] = 5;
    indices[idx++] = 5;
    indices[idx++] = 6;
    indices[idx++] = 6;
    indices[idx++] = 7;
    indices[idx++] = 7;
    indices[idx++] = 4;

    indices[idx++] = 0;
    indices[idx++] = 4;
    indices[idx++] = 1;
    indices[idx++] = 5;
    indices[idx++] = 2;
    indices[idx++] = 6;
    indices[idx++] = 3;
    indices[idx++] = 7;

    bx::memCopy(indexBuffer.data, indices, 24 * sizeof(u16));

    RenderingState state;
    state.PartiallyModifyState(BGFX_STATE_PT_LINES);
    state.ApplyState();

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, 8, vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, 24);

    glm::mat4 transform = glm::identity<glm::mat4>();
    bgfx::setTransform(&transform[0][0]);

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
    AssertRelease(instance != nullptr);

    bgfx::submit(FViewId, instance->GetProgram()->ProgramHandle());
}

IMPLEMENT_POOL_ALLOCATED(DrawAABBCommand);

//----------------------------------------------------------------
//          DrawFrustumCommand
//----------------------------------------------------------------
class DrawFrustumCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(DrawFrustumCommand);

public:
    DrawFrustumCommand(const u16 parViewId,
          const MaterialInstanceHandle& parMaterialInstanceHandle,
          const glm::mat4& parWorldViewTransform,
          const glm::mat4& parProjectionMatrix,
          const u32 parColor = 0xFFFFFFFF);
    virtual ~DrawFrustumCommand();

    virtual void SubmitCommand() const override;

private:
    glm::mat4 FViewWorldTransform;
    glm::mat4 FInverseProjectionMatrix;
    u32 FColor;
    const MaterialInstanceHandle& FMaterialInstanceHandle;
};

DrawFrustumCommand::DrawFrustumCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4& parWorldViewTransform,
      const glm::mat4& parProjectionMatrix,
      const u32 parColor /*= 0xFFFFFFFF*/)
    : IDrawCommand(parViewId)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
    , FViewWorldTransform(glm::inverse(parWorldViewTransform))
    , FInverseProjectionMatrix(glm::inverse(parProjectionMatrix))
    , FColor(parColor)
{
}

DrawFrustumCommand::~DrawFrustumCommand()
{
}

void DrawFrustumCommand::SubmitCommand() const
{
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;

    const bgfx::VertexLayout layout = VertexPositionColorN<1>().GetVertexLayout();
    u32 availableVertices = bgfx::getAvailTransientVertexBuffer(8, layout);
    AssertRelease(availableVertices == 8);
    u32 availableIndices = bgfx::getAvailTransientIndexBuffer(24);
    AssertRelease(availableIndices == 24);

    bgfx::allocTransientBuffers(&vertexBuffer, layout, 8, &indexBuffer, 24);

    VertexPositionColorN<1> vertices[8];
    vertices[0].FPosition = glm::vec3(-1.0f, -1.0f, -1.0f);
    vertices[1].FPosition = glm::vec3(1.0f, -1.0f, -1.0f);
    vertices[2].FPosition = glm::vec3(1.0f, -1.0f, 1.0f);
    vertices[3].FPosition = glm::vec3(-1.0f, -1.0f, 1.0f);

    vertices[4].FPosition = glm::vec3(-1.0f, 1.0f, -1.0f);
    vertices[5].FPosition = glm::vec3(1.0f, 1.0f, -1.0f);
    vertices[6].FPosition = glm::vec3(1.0f, 1.0f, 1.0f);
    vertices[7].FPosition = glm::vec3(-1.0f, 1.0f, 1.0f);

    forrange(i, 0, 8)
    {
        vertices[i].FColor[0] = FColor;
        glm::vec4 worldVertex = glm::vec4(vertices[i].FPosition, 1.0f);
        worldVertex = FInverseProjectionMatrix * worldVertex;
        worldVertex = worldVertex / worldVertex.w;
        vertices[i].FPosition = FViewWorldTransform * worldVertex;
    }

    bx::memCopy(vertexBuffer.data, vertices, sizeof(vertices));

    u16 indices[24];

    u32 idx = 0;

    indices[idx++] = 0;
    indices[idx++] = 1;
    indices[idx++] = 1;
    indices[idx++] = 2;
    indices[idx++] = 2;
    indices[idx++] = 3;
    indices[idx++] = 3;
    indices[idx++] = 0;

    indices[idx++] = 4;
    indices[idx++] = 5;
    indices[idx++] = 5;
    indices[idx++] = 6;
    indices[idx++] = 6;
    indices[idx++] = 7;
    indices[idx++] = 7;
    indices[idx++] = 4;

    indices[idx++] = 0;
    indices[idx++] = 4;
    indices[idx++] = 1;
    indices[idx++] = 5;
    indices[idx++] = 2;
    indices[idx++] = 6;
    indices[idx++] = 3;
    indices[idx++] = 7;

    bx::memCopy(indexBuffer.data, indices, 24 * sizeof(u16));

    RenderingState state;
    state.PartiallyModifyState(BGFX_STATE_PT_LINES);
    state.ApplyState();

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, 8, vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, 24);

    glm::mat4 transform = glm::identity<glm::mat4>();
    bgfx::setTransform(&transform[0][0]);

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
    AssertRelease(instance != nullptr);

    bgfx::submit(FViewId, instance->GetProgram()->ProgramHandle());
}

IMPLEMENT_POOL_ALLOCATED(DrawFrustumCommand);

//----------------------------------------------------------------
//          DrawMeshCommand
//----------------------------------------------------------------
class DrawMeshCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(DrawMeshCommand);

public:
    DrawMeshCommand(const u16 parViewId,
          const MeshHandle& parMeshHandle,
          const MaterialInstanceHandle& parMaterialInstanceHandle,
          const glm::mat4& parTransform = glm::identity<glm::mat4>());
    virtual ~DrawMeshCommand();

    virtual void SubmitCommand() const override;

private:
    glm::mat4 FTransform;
    const MeshHandle& FMeshHandle;
    const MaterialInstanceHandle& FMaterialInstanceHandle;
};

IMPLEMENT_POOL_ALLOCATED(DrawMeshCommand);
DrawMeshCommand::DrawMeshCommand(const u16 parViewId,
      const MeshHandle& parMeshHandle,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4& parTransform /*= glm::identity<glm::mat4>()*/)
    : IDrawCommand(parViewId)
    , FMeshHandle(parMeshHandle)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
    , FTransform(parTransform)
{
    AssertRelease(FMeshHandle.IsValid());
    AssertRelease(FMaterialInstanceHandle.IsValid());
}

DrawMeshCommand::~DrawMeshCommand()
{
}

void DrawMeshCommand::SubmitCommand() const
{
    Rendering::IMesh* mesh = Rendering::MeshManager::Instance().GetMesh(FMeshHandle);
    AssertRelease(mesh != nullptr);

    bgfx::setVertexBuffer(0, mesh->GetVertexBufferHandle());
    bgfx::setIndexBuffer(mesh->GetIndexBufferHandle());

    bgfx::setTransform(&FTransform[0][0]);

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
    AssertRelease(instance != nullptr);

    RenderingState state;
    state.ApplyState();

    bgfx::submit(FViewId, instance->GetProgram()->ProgramHandle());
}

//----------------------------------------------------------------
//          DrawCommandBuffer
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(DrawCommandBuffer);

DrawCommandBuffer::DrawCommandBuffer(const u16 parViewId)
    : FViewId(parViewId)
{
}

DrawCommandBuffer::~DrawCommandBuffer()
{
}

void DrawCommandBuffer::reserve(u32 parSize)
{
    FCommandVector.reserve(parSize);
}

void DrawCommandBuffer::SetViewTranform(const glm::mat4& parViewTransform, const glm::mat4& parProjection)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new SetViewTranformCommand(FViewId, parViewTransform, parProjection)));
}

void DrawCommandBuffer::DrawMesh(const MeshHandle& parMeshHandle,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4& parTransform /*= glm::identity<glm::mat4>()*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawMeshCommand(FViewId, parMeshHandle, parMaterialInstanceHandle, parTransform)));
}

void DrawCommandBuffer::DrawAABB(const MaterialInstanceHandle& parMaterialInstanceHandle, const glm::vec3& parMin, const glm::vec3& parMax, const u32 parColor /*= 0xFFFFFFFF*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawAABBCommand(FViewId, parMaterialInstanceHandle, parMin, parMax, parColor)));
}

void DrawCommandBuffer::DrawFrustum(const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4& parWorldViewTransform,
      const glm::mat4& parProjectionMatrix,
      const u32 parColor /*= 0xFFFFFFFF*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawFrustumCommand(FViewId, parMaterialInstanceHandle, parWorldViewTransform, parProjectionMatrix, parColor)));
}

void DrawCommandBuffer::Submit()
{
    foreachitem(command, FCommandVector) { command->SubmitCommand(); }
}

} // namespace Rendering
} // namespace ECSEngine