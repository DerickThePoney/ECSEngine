#include "stdafx.h"

#include "DrawCommands.h"

#include "Common/Camera.h"
#include "Common/Frustum.h"
#include "Common/MeshStreamingData.h"
#include "FeedbackParameters.h"
#include "Material.h"
#include "MaterialManager.h"
#include "Mesh.h"
#include "MeshManager.h"
#include "MeshUtils.h"
#include "RenderingState.h"
#include "Skeletton.h"
#include "VertexLayout.h"

#include <bx/bx.h>

namespace ECSEngine
{
namespace Rendering
{
//----------------------------------------------------------------
//          SetDebugMarker
//----------------------------------------------------------------
class SetDebugMarkerCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(SetDebugMarkerCommand);

public:
    SetDebugMarkerCommand(const u16 parViewId, const std::string& parDebugMarker);
    virtual ~SetDebugMarkerCommand();

    virtual void SubmitCommand() const override;

private:
    std::string FDebugMarker;
};

IMPLEMENT_POOL_ALLOCATED(SetDebugMarkerCommand);
SetDebugMarkerCommand::SetDebugMarkerCommand(const u16 parViewId, const std::string& parDebugMarker)
    : IDrawCommand(parViewId)
    , FDebugMarker(parDebugMarker)
{
}

SetDebugMarkerCommand::~SetDebugMarkerCommand()
{
}

void SetDebugMarkerCommand::SubmitCommand() const
{
    bgfx::setMarker(FDebugMarker.c_str());
}

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
    glm::mat4 inv = glm::inverse(FViewTransform);
    glm::mat4 inv2 = glm::inverse(FProjection);
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
          const bool parDrawAsCube,
          const u32 parColor = 0xFFFFFFFF);
    virtual ~DrawAABBCommand();

    virtual void SubmitCommand() const override;

private:
    glm::vec3 FMin;
    glm::vec3 FMax;
    bool FDrawAsCube;
    u32 FColor;
    const MaterialInstanceHandle& FMaterialInstanceHandle;
};

DrawAABBCommand::DrawAABBCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::vec3& parMin,
      const glm::vec3& parMax,
      const bool parDrawAsCube,
      const u32 parColor /*= 0xFFFFFFFF*/)
    : IDrawCommand(parViewId)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
    , FColor(parColor)
    , FMin(parMin)
    , FMax(parMax)
    , FDrawAsCube(parDrawAsCube)
{
}

DrawAABBCommand::~DrawAABBCommand()
{
}

void DrawAABBCommand::SubmitCommand() const
{
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;

    VertexLayoutHash hash;
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    u32 availableVertices = bgfx::getAvailTransientVertexBuffer(8, layout);
    AssertRelease(availableVertices == 8);

    bgfx::allocTransientVertexBuffer(&vertexBuffer, 8, layout);

    VertexDataStream stream(8, hash.GetByteSize(), hash);

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, FMin);
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(FMax.x, FMin.y, FMin.z));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(FMax.x, FMin.y, FMax.z));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(FMin.x, FMin.y, FMax.z));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(FMin.x, FMax.y, FMin.z));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(FMax.x, FMax.y, FMin.z));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, FMax);
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(FMin.x, FMax.y, FMax.z));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    bx::memCopy(vertexBuffer.data, stream.GetData(), stream.GetByteSize());

    u32 nbIndices = 0;

    if (FDrawAsCube)
    {
        nbIndices = bgfx::getAvailTransientIndexBuffer(36);
        AssertRelease(nbIndices == 36);

        bgfx::allocTransientIndexBuffer(&indexBuffer, 36);

        u16 indices[36] = {
            7,
            6,
            3,
            6,
            2,
            3,
            4,
            0,
            5,
            5,
            0,
            1,
            7,
            3,
            4,
            4,
            3,
            0,
            6,
            5,
            2,
            5,
            1,
            2,
            7,
            4,
            6,
            4,
            5,
            6,
            3,
            2,
            0,
            0,
            2,
            1,
        };

        bx::memCopy(indexBuffer.data, indices, 36 * sizeof(u16));
    }
    else
    {
        nbIndices = bgfx::getAvailTransientIndexBuffer(24);
        AssertRelease(nbIndices == 24);

        bgfx::allocTransientIndexBuffer(&indexBuffer, 24);

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
    }

    RenderingState state;
    if (!FDrawAsCube)
        state.PartiallyModifyState(BGFX_STATE_PT_LINES);
    state.ApplyState();

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, 8, vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, nbIndices);

    glm::mat4 transform = glm::identity<glm::mat4>();
    bgfx::setTransform(&transform[0][0]);

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
    AssertRelease(instance != nullptr);

    bgfx::submit(FViewId, instance->GetProgram()->ProgramHandle());
}

IMPLEMENT_POOL_ALLOCATED(DrawAABBCommand);

//----------------------------------------------------------------
//          DrawLines3DCommand
//----------------------------------------------------------------

class DrawLines3DCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(DrawLines3DCommand);

public:
    DrawLines3DCommand(const u16 parViewId,
          const MaterialInstanceHandle& parMaterialInstanceHandle,
          const glm::vec3* parVertices,
          const u32 parVerticesSize,
          const u32 parColor,
          const bool parClose);
    virtual ~DrawLines3DCommand();

    virtual void SubmitCommand() const override;

private:
    const glm::vec3* FVertices;
    u32 FVerticesSize;
    u32 FColor;
    bool FClose;
    const MaterialInstanceHandle& FMaterialHandle;
};

DrawLines3DCommand::DrawLines3DCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::vec3* parVertices,
      const u32 parVerticesSize,
      const u32 parColor,
      const bool parClose)
    : IDrawCommand(parViewId)
    , FMaterialHandle(parMaterialInstanceHandle)
    , FVertices(parVertices)
    , FVerticesSize(parVerticesSize)
    , FColor(parColor)
    , FClose(parClose)
{
}

DrawLines3DCommand::~DrawLines3DCommand()
{
}

void DrawLines3DCommand::SubmitCommand() const
{
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;

    VertexLayoutHash hash;
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    const u32 availableVertices = bgfx::getAvailTransientVertexBuffer(FVerticesSize, layout);
    AlwaysCheckedAssert(availableVertices == FVerticesSize);

    bgfx::allocTransientVertexBuffer(&vertexBuffer, FVerticesSize, layout);

    VertexDataStream stream(FVerticesSize, hash.GetByteSize(), hash);

    forrange(i, 0, FVerticesSize)
    {
        stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, FVertices[i]);
        stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
        stream.Advance();
    }

    std::vector<u16> indices;
    forrange(i, 0, FVerticesSize - 1)
    {
        indices.push_back((u16)i);
        indices.push_back((u16)(i + 1));
    }

    if (FClose)
    {
        indices.push_back((u16)(FVerticesSize - 1));
        indices.push_back((u16)0);
    }
    const u32 availableIndices = bgfx::getAvailTransientIndexBuffer((u32)indices.size());
    AlwaysCheckedAssert(availableIndices == (u32)indices.size());
    bgfx::allocTransientIndexBuffer(&indexBuffer, (u32)indices.size());
    bx::memCopy(indexBuffer.data, indices.data(), (u32)indices.size() * sizeof(u16));
    bx::memCopy(vertexBuffer.data, stream.GetData(), stream.GetByteSize());

    RenderingState state;
    state.PartiallyModifyState(BGFX_STATE_PT_LINES);
    state.ApplyState();

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, FVerticesSize, vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, (u32)indices.size());

    glm::mat4 transform = glm::identity<glm::mat4>();
    bgfx::setTransform(&transform[0][0]);

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialHandle);
    AssertRelease(instance != nullptr);

    bgfx::submit(FViewId, instance->GetProgram()->ProgramHandle());
}

IMPLEMENT_POOL_ALLOCATED(DrawLines3DCommand);

//----------------------------------------------------------------
//          DrawLines2DCommand
//----------------------------------------------------------------

class DrawLines2DCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(DrawLines2DCommand);

public:
    DrawLines2DCommand(const u16 parViewId,
          const MaterialInstanceHandle& parMaterialInstanceHandle,
          const glm::vec2* parVertices,
          const u32 parVerticesSize,
          const float parHeight,
          const u32 parColor,
          const bool parClose);
    virtual ~DrawLines2DCommand();

    virtual void SubmitCommand() const override;

protected:
    virtual const glm::vec2* Vertices() const { return FVertices; }

private:
    const glm::vec2* FVertices;
    u32 FVerticesSize;
    float FHeight;
    u32 FColor;
    bool FClose;
    const MaterialInstanceHandle& FMaterialHandle;
};

DrawLines2DCommand::DrawLines2DCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::vec2* parVertices,
      const u32 parVerticesSize,
      const float parHeight,
      const u32 parColor,
      const bool parClose)
    : IDrawCommand(parViewId)
    , FMaterialHandle(parMaterialInstanceHandle)
    , FVertices(parVertices)
    , FVerticesSize(parVerticesSize)
    , FHeight(parHeight)
    , FColor(parColor)
    , FClose(parClose)
{
}

DrawLines2DCommand::~DrawLines2DCommand()
{
}

void DrawLines2DCommand::SubmitCommand() const
{
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;

    VertexLayoutHash hash;
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    const u32 availableVertices = bgfx::getAvailTransientVertexBuffer(FVerticesSize, layout);
    AlwaysCheckedAssert(availableVertices == FVerticesSize);

    bgfx::allocTransientVertexBuffer(&vertexBuffer, FVerticesSize, layout);

    VertexDataStream stream(FVerticesSize, hash.GetByteSize(), hash);

    forrange(i, 0, FVerticesSize)
    {
        stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(Vertices()[i].x, FHeight, Vertices()[i].y));
        stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
        stream.Advance();
    }

    std::vector<u16> indices;
    forrange(i, 0, FVerticesSize - 1)
    {
        indices.push_back((u16)i);
        indices.push_back((u16)(i + 1));
    }

    if (FClose)
    {
        indices.push_back((u16)(FVerticesSize - 1));
        indices.push_back((u16)0);
    }
    const u32 availableIndices = bgfx::getAvailTransientIndexBuffer((u32)indices.size());
    AlwaysCheckedAssert(availableIndices == (u32)indices.size());
    bgfx::allocTransientIndexBuffer(&indexBuffer, (u32)indices.size());
    bx::memCopy(indexBuffer.data, indices.data(), (u32)indices.size() * sizeof(u16));
    bx::memCopy(vertexBuffer.data, stream.GetData(), stream.GetByteSize());

    RenderingState state;
    state.PartiallyModifyState(BGFX_STATE_PT_LINES);
    state.ApplyState();

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, FVerticesSize, vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, (u32)indices.size());

    glm::mat4 transform = glm::identity<glm::mat4>();
    bgfx::setTransform(&transform[0][0]);

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialHandle);
    AssertRelease(instance != nullptr);

    bgfx::submit(FViewId, instance->GetProgram()->ProgramHandle());
}

IMPLEMENT_POOL_ALLOCATED(DrawLines2DCommand);

//----------------------------------------------------------------
//          DrawLines2DKeepDataCommand
//----------------------------------------------------------------

class DrawLines2DKeepDataCommand : public DrawLines2DCommand
{
    DECLARE_POOL_ALLOCATED(DrawLines2DKeepDataCommand);

public:
    DrawLines2DKeepDataCommand(const u16 parViewId,
          const MaterialInstanceHandle& parMaterialInstanceHandle,
          const std::vector<glm::vec2>& parVertices,
          const u32 parVerticesSize,
          const float parHeight,
          const u32 parColor,
          const bool parClose);

protected:
    const glm::vec2* Vertices() const override { return FLineVertices.data(); }

private:
    std::vector<glm::vec2> FLineVertices;
};

DrawLines2DKeepDataCommand::DrawLines2DKeepDataCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const std::vector<glm::vec2>& parVertices,
      const u32 parVerticesSize,
      const float parHeight,
      const u32 parColor,
      const bool parClose)
    : FLineVertices(parVertices)
    , DrawLines2DCommand(parViewId, parMaterialInstanceHandle, nullptr, parVerticesSize, parHeight, parColor, parClose)
{
}

IMPLEMENT_POOL_ALLOCATED(DrawLines2DKeepDataCommand);
//----------------------------------------------------------------
//          DrawFrustumCommand
//----------------------------------------------------------------
namespace
{
void PushNormalVertices(const glm::vec3 vertex, const glm::vec3 normal, const u32 color, VertexDataStream& stream)
{
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, vertex);
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, vertex + normal);
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
    stream.Advance();
}
} // namespace

class DrawFrustumCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(DrawFrustumCommand);

public:
    DrawFrustumCommand(const u16 parViewId,
          const MaterialInstanceHandle& parMaterialInstanceHandle,
          const glm::mat4& parWorldViewTransform,
          const glm::mat4& parProjectionMatrix,
          const bool parDrawFrustumNormals = false,
          const u32 parColor = 0xFFFFFFFF,
          const u32 parNormalsColor = 0xFFFFFFFF);
    virtual ~DrawFrustumCommand();

    virtual void SubmitCommand() const override;

private:
    glm::mat4 FViewWorldTransform;
    glm::mat4 FInverseProjectionMatrix;
    u32 FColor;
    u32 FNormalsColor;
    bool FDrawFrustumNormals;
    const MaterialInstanceHandle& FMaterialInstanceHandle;
};

DrawFrustumCommand::DrawFrustumCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4& parWorldViewTransform,
      const glm::mat4& parProjectionMatrix,
      const bool parDrawFrustumNormals /*= false*/,
      const u32 parColor /*= 0xFFFFFFFF*/,
      const u32 parNormalsColor /*= 0xFFFFFFFF*/)
    : IDrawCommand(parViewId)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
    , FViewWorldTransform(parWorldViewTransform)
    , FInverseProjectionMatrix(glm::inverse(parProjectionMatrix))
    , FDrawFrustumNormals(parDrawFrustumNormals)
    , FColor(parColor)
    , FNormalsColor(parNormalsColor)
{
}

DrawFrustumCommand::~DrawFrustumCommand()
{
}

void DrawFrustumCommand::SubmitCommand() const
{
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;

    VertexLayoutHash hash;
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    FrustumCorners frustumCorners;
    frustumCorners.InitFromMatrices(FViewWorldTransform, FInverseProjectionMatrix);
    const MemoryView<const glm::vec4> corners = frustumCorners.GetCorners();

    const u32 wantedVertices = 8 + ((FDrawFrustumNormals) ? 12 : 0);
    const u32 availableVertices = bgfx::getAvailTransientVertexBuffer(wantedVertices, layout);
    AssertRelease(availableVertices == wantedVertices);
    const u32 wantedIndices = 24 + ((FDrawFrustumNormals) ? 12 : 0);
    const u32 availableIndices = bgfx::getAvailTransientIndexBuffer(wantedIndices);
    AssertRelease(availableIndices == wantedIndices);

    bgfx::allocTransientBuffers(&vertexBuffer, layout, wantedVertices, &indexBuffer, wantedIndices);

    VertexDataStream stream(wantedVertices, hash.GetByteSize(), hash);

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(corners[FrustumCorner::NEAR_BOTTOM_LEFT]));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(corners[FrustumCorner::NEAR_BOTTOM_RIGHT]));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(corners[FrustumCorner::FAR_BOTTOM_RIGHT]));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(corners[FrustumCorner::FAR_BOTTOM_LEFT]));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(corners[FrustumCorner::NEAR_TOP_LEFT]));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(corners[FrustumCorner::NEAR_TOP_RIGHT]));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(corners[FrustumCorner::FAR_TOP_RIGHT]));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(corners[FrustumCorner::FAR_TOP_LEFT]));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    if (FDrawFrustumNormals)
    {
        Frustum f;
        f.InitFromCorners(frustumCorners);
        MemoryView<const glm::vec4> planes = f.GetPlanes();

        // NearPlane
        const glm::vec4 nearPlanePoint = (corners[FrustumCorner::NEAR_BOTTOM_LEFT] + corners[FrustumCorner::NEAR_BOTTOM_RIGHT] + corners[FrustumCorner::NEAR_TOP_LEFT] +
                                               corners[FrustumCorner::NEAR_TOP_RIGHT]) /
              4;
        PushNormalVertices(nearPlanePoint, glm::xyz(planes[FrustumPlane::NEAR_PLANE]), FNormalsColor, stream);

        // Farplane
        const glm::vec4 farPlanePoint =
              (corners[FrustumCorner::FAR_BOTTOM_LEFT] + corners[FrustumCorner::FAR_BOTTOM_RIGHT] + corners[FrustumCorner::FAR_TOP_LEFT] + corners[FrustumCorner::FAR_TOP_RIGHT]) /
              4;
        PushNormalVertices(farPlanePoint, glm::xyz(planes[FrustumPlane::FAR_PLANE]), FNormalsColor, stream);

        // LEFTPlane
        const glm::vec4 leftPlanePoint =
              (corners[FrustumCorner::NEAR_BOTTOM_LEFT] + corners[FrustumCorner::NEAR_TOP_LEFT] + corners[FrustumCorner::FAR_TOP_LEFT] + corners[FrustumCorner::FAR_BOTTOM_LEFT]) /
              4;
        PushNormalVertices(leftPlanePoint, glm::xyz(planes[FrustumPlane::LEFT_PLANE]), FNormalsColor, stream);

        // RightPlane
        const glm::vec4 rightPlanePoint = (corners[FrustumCorner::NEAR_BOTTOM_RIGHT] + corners[FrustumCorner::NEAR_TOP_RIGHT] + corners[FrustumCorner::FAR_BOTTOM_RIGHT] +
                                                corners[FrustumCorner::FAR_TOP_RIGHT]) /
              4;
        PushNormalVertices(rightPlanePoint, glm::xyz(planes[FrustumPlane::RIGHT_PLANE]), FNormalsColor, stream);

        // TopPlane
        const glm::vec4 topPlanePoint =
              (corners[FrustumCorner::NEAR_TOP_LEFT] + corners[FrustumCorner::NEAR_TOP_RIGHT] + corners[FrustumCorner::FAR_TOP_LEFT] + corners[FrustumCorner::FAR_TOP_RIGHT]) / 4;
        PushNormalVertices(topPlanePoint, glm::xyz(planes[FrustumPlane::TOP_PLANE]), FNormalsColor, stream);

        // BottomPlane
        const glm::vec4 bottomPlanePoint = (corners[FrustumCorner::NEAR_BOTTOM_LEFT] + corners[FrustumCorner::NEAR_BOTTOM_RIGHT] + corners[FrustumCorner::FAR_BOTTOM_LEFT] +
                                                 corners[FrustumCorner::FAR_BOTTOM_RIGHT]) /
              4;
        PushNormalVertices(bottomPlanePoint, glm::xyz(planes[FrustumPlane::BOTTOM_PLANE]), FNormalsColor, stream);
    }

    bx::memCopy(vertexBuffer.data, stream.GetData(), stream.GetByteSize());

    std::vector<u16> indices(wantedIndices, 0);

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

    if (FDrawFrustumNormals)
    {
        indices[idx++] = 8;
        indices[idx++] = 9;

        indices[idx++] = 10;
        indices[idx++] = 11;

        indices[idx++] = 12;
        indices[idx++] = 13;

        indices[idx++] = 14;
        indices[idx++] = 15;

        indices[idx++] = 16;
        indices[idx++] = 17;

        indices[idx++] = 18;
        indices[idx++] = 19;
    }

    bx::memCopy(indexBuffer.data, indices.data(), wantedIndices * sizeof(u16));

    RenderingState state;
    state.PartiallyModifyState(BGFX_STATE_PT_LINES);
    state.ApplyState();

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, stream.GetSize(), vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, (u32)indices.size());

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
          const SkelettonPose* parSkelettonPose = nullptr,
          const glm::mat4& parTransform = glm::identity<glm::mat4>());
    virtual ~DrawMeshCommand();

    virtual void SubmitCommand() const override;

private:
    glm::mat4 FTransform;
    const SkelettonPose* FSkelettonPose = nullptr;
    MeshHandle FMeshHandle;
    MaterialInstanceHandle FMaterialInstanceHandle;
};

IMPLEMENT_POOL_ALLOCATED(DrawMeshCommand);
DrawMeshCommand::DrawMeshCommand(const u16 parViewId,
      const MeshHandle& parMeshHandle,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const SkelettonPose* parSkelettonPose /*= nullptr*/,
      const glm::mat4& parTransform /*= glm::identity<glm::mat4>()*/)
    : IDrawCommand(parViewId)
    , FMeshHandle(parMeshHandle)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
    , FTransform(parTransform)
    , FSkelettonPose(parSkelettonPose)
{
    AssertRelease(FMeshHandle.IsValid());
    AssertRelease(FMaterialInstanceHandle.IsValid());
}

DrawMeshCommand::~DrawMeshCommand()
{
}

void DrawMeshCommand::SubmitCommand() const
{
    Rendering::Mesh* mesh = Rendering::MeshManager::Instance().GetMesh(FMeshHandle);
    AssertRelease(mesh != nullptr);

    bgfx::setVertexBuffer(0, mesh->GetVertexBufferHandle());
    bgfx::setIndexBuffer(mesh->GetIndexBufferHandle());

    bgfx::setTransform(&FTransform[0][0]);

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
    AssertRelease(instance != nullptr);

    instance->SetTextures();

    if (FSkelettonPose != nullptr)
    {
        Rendering::MaterialManager::SetMat4Uniforms("u_skinningMatrices", FSkelettonPose->SkinningMatrices(), FSkelettonPose->BonesNumber());
    }

    RenderingState state;
    state.ApplyState();

    bgfx::submit(FViewId, instance->GetProgram()->ProgramHandle());
}

//----------------------------------------------------------------
//          DrawVerticesCommand
//----------------------------------------------------------------

class DrawVerticesCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(DrawVerticesCommand);

public:
    DrawVerticesCommand(const u16 parViewId,
          const glm::vec3* parVertices,
          const u32 parVerticesSize,
          const u16* parIndices,
          const u32 parIndicesSize,
          const MaterialInstanceHandle& parMaterialInstanceHandle,
          const glm::mat4& parTransform = glm::identity<glm::mat4>());
    virtual ~DrawVerticesCommand();

    virtual void SubmitCommand() const override;

private:
    const glm::vec3* FVertices;
    const u32 FVerticesSize;
    const u16* FIndices;
    const u32 FIndicesSize;
    glm::mat4 FTransform;
    MaterialInstanceHandle FMaterialInstanceHandle;
};

IMPLEMENT_POOL_ALLOCATED(DrawVerticesCommand);
DrawVerticesCommand::DrawVerticesCommand(const u16 parViewId,
      const glm::vec3* parVertices,
      const u32 parVerticesSize,
      const u16* parIndices,
      const u32 parIndicesSize,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4& parTransform /*= glm::identity<glm::mat4>()*/)
    : IDrawCommand(parViewId)
    , FVertices(parVertices)
    , FVerticesSize(parVerticesSize)
    , FIndices(parIndices)
    , FIndicesSize(parIndicesSize)
    , FTransform(parTransform)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
{
    AssertRelease(FVertices != nullptr);
    AssertRelease(FIndices != nullptr);
}

DrawVerticesCommand::~DrawVerticesCommand()
{
}

void DrawVerticesCommand::SubmitCommand() const
{
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;

    VertexLayoutHash hash;
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    u32 availableVertices = bgfx::getAvailTransientVertexBuffer(FVerticesSize, layout);
    AssertRelease(availableVertices == FVerticesSize);
    u32 availableIndices = bgfx::getAvailTransientIndexBuffer(FIndicesSize);
    AssertRelease(availableIndices == FIndicesSize);

    bgfx::allocTransientBuffers(&vertexBuffer, layout, FVerticesSize, &indexBuffer, FIndicesSize);

    VertexDataStream stream(FVerticesSize, hash.GetByteSize(), hash);

    forrange(i, 0, FVerticesSize) { stream.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, FVertices[i], (u32)i); }

    bx::memCopy(vertexBuffer.data, stream.GetData(), stream.GetByteSize());

    bx::memCopy(indexBuffer.data, FIndices, FIndicesSize * sizeof(u16));

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, FVerticesSize, vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, FIndicesSize);

    bgfx::setTransform(&FTransform[0][0]);

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
    AssertRelease(instance != nullptr);

    bgfx::submit(FViewId, instance->GetProgram()->ProgramHandle());
}

//----------------------------------------------------------------
//          DrawCircle
//----------------------------------------------------------------

class DrawCircleCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(DrawCircleCommand);

public:
    DrawCircleCommand(const u16 parViewId,
          const CircleFeedbackParameters& parParams,
          const MaterialInstanceHandle& parMaterialInstanceHandle,
          const glm::mat4 parTransform = glm::identity<glm::mat4>());
    virtual ~DrawCircleCommand();

    virtual void SubmitCommand() const override;

private:
    CircleFeedbackParameters FParams;
    glm::mat4 FTransform;
    MaterialInstanceHandle FMaterialInstanceHandle;
};

IMPLEMENT_POOL_ALLOCATED(DrawCircleCommand);

DrawCircleCommand::DrawCircleCommand(const u16 parViewId,
      const CircleFeedbackParameters& parParams,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4 parTransform /*= glm::identity<glm::mat4>()*/)
    : IDrawCommand(parViewId)
    , FParams(parParams)
    , FTransform(parTransform)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
{
}

DrawCircleCommand::~DrawCircleCommand()
{
}

void DrawCircleCommand::SubmitCommand() const
{
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;

    VertexLayoutHash hash;
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    u32 availableVertices = bgfx::getAvailTransientVertexBuffer(4, layout);
    AssertRelease(availableVertices == 4);
    u32 availableIndices = bgfx::getAvailTransientIndexBuffer(6);
    AssertRelease(availableIndices == 6);

    bgfx::allocTransientBuffers(&vertexBuffer, layout, 4, &indexBuffer, 6);

    const u8 a = (u8)(FParams.Color.w * 255.f);
    const u8 b = (u8)(FParams.Color.z * 255.f);
    const u8 g = (u8)(FParams.Color.y * 255.f);
    const u8 r = (u8)(FParams.Color.x * 255.f);
    const u32 color = a << 24 | b << 16 | g << 8 | r;

    const float effectiveRange = FParams.Range * 1.1f;

    VertexDataStream stream(4, hash.GetByteSize(), hash);
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(-effectiveRange, 0.01f, -effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(effectiveRange, 0.01f, -effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(effectiveRange, 0.01f, effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(-effectiveRange, 0.01f, effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
    stream.Advance();

    std::array<u16, 6> indices;
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    bx::memCopy(vertexBuffer.data, stream.GetData(), stream.GetByteSize());

    bx::memCopy(indexBuffer.data, indices.data(), 6 * sizeof(u16));

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, 4, vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, 6);

    bgfx::setTransform(&FTransform[0][0]);

    Rendering::RenderingState state(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_CULL_CW | BGFX_STATE_BLEND_ALPHA);
    state.ApplyState();

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
    AssertRelease(instance != nullptr);
    Rendering::MaterialManager::SetVec4Uniform("u_circleRadius", glm::vec4(FParams.Range, FParams.Range - FParams.Thickness, 0.f, 0.f));

    bgfx::submit(FViewId, instance->GetProgram()->ProgramHandle());
}

//----------------------------------------------------------------
//          DrawCircularChunkCommand
//----------------------------------------------------------------

class DrawCircularChunkCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(DrawCircularChunkCommand);

public:
    DrawCircularChunkCommand(const u16 parViewId,
          const CircularGridChunkFeedbackParameters& parParameters,
          const MaterialInstanceHandle& parMaterialInstanceHandle,
          const glm::mat4 parTransform = glm::identity<glm::mat4>());
    virtual ~DrawCircularChunkCommand();

    virtual void SubmitCommand() const override;

private:
    CircularGridChunkFeedbackParameters FParameters;
    glm::mat4 FTransform;
    MaterialInstanceHandle FMaterialInstanceHandle;
};

IMPLEMENT_POOL_ALLOCATED(DrawCircularChunkCommand);

DrawCircularChunkCommand::DrawCircularChunkCommand(const u16 parViewId,
      const CircularGridChunkFeedbackParameters& parParameters,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4 parTransform /*= glm::identity<glm::mat4>()*/)
    : IDrawCommand(parViewId)
    , FParameters(parParameters)
    , FTransform(parTransform)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
{
    AlwaysCheckedAssert(FParameters.OuterCircleRadius > FParameters.InnerCircleRadius);
}

DrawCircularChunkCommand::~DrawCircularChunkCommand()
{
}

void DrawCircularChunkCommand::SubmitCommand() const
{
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;

    VertexLayoutHash hash;
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    u32 availableVertices = bgfx::getAvailTransientVertexBuffer(4, layout);
    AssertRelease(availableVertices == 4);
    u32 availableIndices = bgfx::getAvailTransientIndexBuffer(6);
    AssertRelease(availableIndices == 6);

    bgfx::allocTransientBuffers(&vertexBuffer, layout, 4, &indexBuffer, 6);

    const float effectiveRange = FParameters.OuterCircleRadius * 1.1f;

    VertexDataStream stream(4, hash.GetByteSize(), hash);
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(-effectiveRange, 0.01f, -effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FParameters.Color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(effectiveRange, 0.01f, -effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FParameters.Color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(effectiveRange, 0.01f, effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FParameters.Color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(-effectiveRange, 0.01f, effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FParameters.Color);
    stream.Advance();

    std::array<u16, 6> indices;
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    bx::memCopy(vertexBuffer.data, stream.GetData(), stream.GetByteSize());

    bx::memCopy(indexBuffer.data, indices.data(), 6 * sizeof(u16));

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, 4, vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, 6);

    bgfx::setTransform(&FTransform[0][0]);

    Rendering::RenderingState state(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_CULL_CW | BGFX_STATE_BLEND_ALPHA | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
    state.ApplyState();

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
    AssertRelease(instance != nullptr);
    Rendering::MaterialManager::SetVec4Uniform("u_innerOuterCircles",
          glm::vec4(FParameters.InnerCircleRadius + 0.5f * FParameters.Thickness, FParameters.InnerCircleRadius, FParameters.OuterCircleRadius,
                FParameters.OuterCircleRadius - 0.5f * FParameters.Thickness));
    Rendering::MaterialManager::SetVec4Uniform("u_arcAngle", glm::vec4(FParameters.ArcAngle));

    bgfx::submit(FViewId, instance->GetProgram()->ProgramHandle());
}

//----------------------------------------------------------------
//          BlitWithMaterialCommand
//----------------------------------------------------------------

class BlitWithMaterialCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(BlitWithMaterialCommand);

public:
    BlitWithMaterialCommand(const u16 parViewId, const MaterialInstanceHandle& parMaterialInstanceHandle);
    virtual ~BlitWithMaterialCommand();

    virtual void SubmitCommand() const override;

private:
    MaterialInstanceHandle FMaterialInstanceHandle;
};

IMPLEMENT_POOL_ALLOCATED(BlitWithMaterialCommand);

BlitWithMaterialCommand::BlitWithMaterialCommand(const u16 parViewId, const MaterialInstanceHandle& parMaterialInstanceHandle)
    : IDrawCommand(parViewId)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
{
}

BlitWithMaterialCommand::~BlitWithMaterialCommand()
{
}

void BlitWithMaterialCommand::SubmitCommand() const
{
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;

    VertexLayoutHash hash;
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSTION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_UVS, true);
    hash.SetUVsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    u32 availableVertices = bgfx::getAvailTransientVertexBuffer(4, layout);
    AssertRelease(availableVertices == 4);
    u32 availableIndices = bgfx::getAvailTransientIndexBuffer(6);
    AssertRelease(availableIndices == 6);

    bgfx::allocTransientBuffers(&vertexBuffer, layout, 4, &indexBuffer, 6);

    VertexDataStream stream(4, hash.GetByteSize(), hash);
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(-1.f, -1.f, 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, glm::vec2(0.f, 1.f));
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(1.f, -1.f, 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, glm::vec2(1.f, 1.f));
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(1.f, 1.f, 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, glm::vec2(1.f, 0.f));
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, glm::vec3(-1.f, 1.f, 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, glm::vec2(0.f, 0.f));
    stream.Advance();

    std::array<u16, 6> indices;
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    bx::memCopy(vertexBuffer.data, stream.GetData(), stream.GetByteSize());

    bx::memCopy(indexBuffer.data, indices.data(), 6 * sizeof(u16));

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, 4, vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, 6);

    /*Rendering::RenderingState state(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_CULL_CW);
    state.ApplyState();*/

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
    AssertRelease(instance != nullptr);

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

void DrawCommandBuffer::clear()
{
    FCommandVector.clear();
}

void DrawCommandBuffer::SetDebugMarker(const std::string& parDebugMarker)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new SetDebugMarkerCommand(FViewId, parDebugMarker)));
}

void DrawCommandBuffer::SetViewTranform(const glm::mat4& parViewTransform, const glm::mat4& parProjection)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new SetViewTranformCommand(FViewId, parViewTransform, parProjection)));
}

void DrawCommandBuffer::DrawVertices(const glm::vec3* parVertices,
      const u32 parVerticesSize,
      const u16* parIndices,
      const u32 parIndicesSize,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4& parTransform)
{
    FCommandVector.push_back(
          std::unique_ptr<IDrawCommand>(new DrawVerticesCommand(FViewId, parVertices, parVerticesSize, parIndices, parIndicesSize, parMaterialInstanceHandle, parTransform)));
}

void DrawCommandBuffer::DrawMesh(const MeshHandle& parMeshHandle,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4& parTransform /*= glm::identity<glm::mat4>()*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawMeshCommand(FViewId, parMeshHandle, parMaterialInstanceHandle, nullptr, parTransform)));
}

void DrawCommandBuffer::DrawMeshWithPose(const SkelettonPose* parSkelettonPose,
      const MeshHandle& parMeshHandle,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4& parTransform /*= glm::identity<glm::mat4>()*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawMeshCommand(FViewId, parMeshHandle, parMaterialInstanceHandle, parSkelettonPose, parTransform)));
}

void DrawCommandBuffer::DrawAABB(const MaterialInstanceHandle& parMaterialInstanceHandle, const glm::vec3& parMin, const glm::vec3& parMax, const u32 parColor /*= 0xFFFFFFFF*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawAABBCommand(FViewId, parMaterialInstanceHandle, parMin, parMax, false, parColor)));
}

void DrawCommandBuffer::DrawAABBAsCube(const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::vec3& parMin,
      const glm::vec3& parMax,
      const u32 parColor /*= 0xFFFFFFFF*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawAABBCommand(FViewId, parMaterialInstanceHandle, parMin, parMax, true, parColor)));
}

void DrawCommandBuffer::DrawFrustum(const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4& parWorldViewTransform,
      const glm::mat4& parProjectionMatrix,
      const bool parDrawFrustumNormals /*= false*/,
      const u32 parColor /*= 0xFFFFFFFF*/,
      const u32 parNormalsColor /*= 0xFFFFFFFF*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(
          new DrawFrustumCommand(FViewId, parMaterialInstanceHandle, parWorldViewTransform, parProjectionMatrix, parDrawFrustumNormals, parColor, parNormalsColor)));
}

void DrawCommandBuffer::DrawLines(const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::vec3* parVertices,
      const u32 parVerticesSize,
      const u32 parColor,
      const bool parClose)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawLines3DCommand(FViewId, parMaterialInstanceHandle, parVertices, parVerticesSize, parColor, parClose)));
}

void DrawCommandBuffer::DrawLines(const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::vec2* parVertices,
      const u32 parVerticesSize,
      const float parHeight,
      const u32 parColor,
      const bool parClose)
{
    FCommandVector.push_back(
          std::unique_ptr<IDrawCommand>(new DrawLines2DCommand(FViewId, parMaterialInstanceHandle, parVertices, parVerticesSize, parHeight, parColor, parClose)));
}

void DrawCommandBuffer::DrawLines(const MaterialInstanceHandle& parMaterialInstanceHandle,
      const std::vector<glm::vec2>& parVertices,
      const u32 parVerticesSize,
      const float parHeight,
      const u32 parColor,
      const bool parClose)
{
    FCommandVector.push_back(
          std::unique_ptr<IDrawCommand>(new DrawLines2DKeepDataCommand(FViewId, parMaterialInstanceHandle, parVertices, parVerticesSize, parHeight, parColor, parClose)));
}

void DrawCommandBuffer::DrawCircle(const MaterialInstanceHandle& parMaterialInstanceHandle,
      const CircleFeedbackParameters& parParams,
      const glm::mat4& parTransform /*= glm::identity<glm::mat4>()*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawCircleCommand(FViewId, parParams, parMaterialInstanceHandle, parTransform)));
}

void DrawCommandBuffer::DrawCircularChunk(const CircularGridChunkFeedbackParameters& parParameters,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const glm::mat4 parTransform /*= glm::identity<glm::mat4>()*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawCircularChunkCommand(FViewId, parParameters, parMaterialInstanceHandle, parTransform)));
}

void DrawCommandBuffer::BlitWithMaterial(const MaterialInstanceHandle& parMaterialInstanceHandle)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new BlitWithMaterialCommand(FViewId, parMaterialInstanceHandle)));
}

void DrawCommandBuffer::Submit()
{
    foreachitem(command, FCommandVector) { command->SubmitCommand(); }
}

} // namespace Rendering
} // namespace ECSEngine
