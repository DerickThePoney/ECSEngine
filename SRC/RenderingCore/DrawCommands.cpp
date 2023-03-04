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
    SetViewTranformCommand(const u16 parViewId, const mat4& parViewTransform, const mat4& parProjection);
    virtual ~SetViewTranformCommand();

    virtual void SubmitCommand() const override;

private:
    mat4 FViewTransform;
    mat4 FProjection;
};

IMPLEMENT_POOL_ALLOCATED(SetViewTranformCommand);
SetViewTranformCommand::SetViewTranformCommand(const u16 parViewId, const mat4& parViewTransform, const mat4& parProjection)
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
    bgfx::setViewTransform(FViewId, &FViewTransform.FValues[0], &FProjection.FValues[0]);
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
          const vec3& parMin,
          const vec3& parMax,
          const bool parDrawAsCube,
          const u32 parColor = 0xFFFFFFFF);
    virtual ~DrawAABBCommand();

    virtual void SubmitCommand() const override;

private:
    vec3 FMin;
    vec3 FMax;
    bool FDrawAsCube;
    u32 FColor;
    const MaterialInstanceHandle& FMaterialInstanceHandle;
};

DrawAABBCommand::DrawAABBCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const vec3& parMin,
      const vec3& parMax,
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
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    u32 availableVertices = bgfx::getAvailTransientVertexBuffer(8, layout);
    AssertRelease(availableVertices == 8);

    bgfx::allocTransientVertexBuffer(&vertexBuffer, 8, layout);

    VertexDataStream stream(8, hash.GetByteSize(), hash);

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, FMin);
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(FMax.x, FMin.y, FMin.z));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(FMax.x, FMin.y, FMax.z));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(FMin.x, FMin.y, FMax.z));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(FMin.x, FMax.y, FMin.z));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(FMax.x, FMax.y, FMin.z));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, FMax);
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(FMin.x, FMax.y, FMax.z));
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

    mat4 transform = mat4::Identity();
    bgfx::setTransform(&transform.FValues[0]);

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
          const vec3* parVertices,
          const u32 parVerticesSize,
          const u32 parColor,
          const bool parClose);
    virtual ~DrawLines3DCommand();

    virtual void SubmitCommand() const override;

private:
    const vec3* FVertices;
    u32 FVerticesSize;
    u32 FColor;
    bool FClose;
    const MaterialInstanceHandle& FMaterialHandle;
};

DrawLines3DCommand::DrawLines3DCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const vec3* parVertices,
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
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    const u32 availableVertices = bgfx::getAvailTransientVertexBuffer(FVerticesSize, layout);
    AlwaysCheckedAssert(availableVertices == FVerticesSize);

    bgfx::allocTransientVertexBuffer(&vertexBuffer, FVerticesSize, layout);

    VertexDataStream stream(FVerticesSize, hash.GetByteSize(), hash);

    forrange(i, 0, FVerticesSize)
    {
        stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, FVertices[i]);
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

    mat4 transform = mat4::Identity();
    bgfx::setTransform(&transform.FValues[0]);

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
          const vec2* parVertices,
          const u32 parVerticesSize,
          const float parHeight,
          const u32 parColor,
          const bool parClose);
    virtual ~DrawLines2DCommand();

    virtual void SubmitCommand() const override;

protected:
    virtual const vec2* Vertices() const { return FVertices; }

private:
    const vec2* FVertices;
    u32 FVerticesSize;
    float FHeight;
    u32 FColor;
    bool FClose;
    const MaterialInstanceHandle& FMaterialHandle;
};

DrawLines2DCommand::DrawLines2DCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const vec2* parVertices,
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
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    const u32 availableVertices = bgfx::getAvailTransientVertexBuffer(FVerticesSize, layout);
    AlwaysCheckedAssert(availableVertices == FVerticesSize);

    bgfx::allocTransientVertexBuffer(&vertexBuffer, FVerticesSize, layout);

    VertexDataStream stream(FVerticesSize, hash.GetByteSize(), hash);

    forrange(i, 0, FVerticesSize)
    {
        stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(Vertices()[i].x, FHeight, Vertices()[i].y));
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

    mat4 transform = mat4::Identity();
    bgfx::setTransform(&transform.FValues[0]);

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
          const std::vector<vec2>& parVertices,
          const u32 parVerticesSize,
          const float parHeight,
          const u32 parColor,
          const bool parClose);

protected:
    const vec2* Vertices() const override { return FLineVertices.data(); }

private:
    std::vector<vec2> FLineVertices;
};

DrawLines2DKeepDataCommand::DrawLines2DKeepDataCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const std::vector<vec2>& parVertices,
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
void PushNormalVertices(const vec3 vertex, const vec3 normal, const u32 color, VertexDataStream& stream)
{
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vertex);
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vertex + normal);
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
          const mat4& parWorldViewTransform,
          const mat4& parProjectionMatrix,
          const bool parDrawFrustumNormals = false,
          const u32 parColor = 0xFFFFFFFF,
          const u32 parNormalsColor = 0xFFFFFFFF);
    virtual ~DrawFrustumCommand();

    virtual void SubmitCommand() const override;

private:
    mat4 FViewWorldTransform;
    mat4 FInverseProjectionMatrix;
    u32 FColor;
    u32 FNormalsColor;
    bool FDrawFrustumNormals;
    const MaterialInstanceHandle& FMaterialInstanceHandle;
};

DrawFrustumCommand::DrawFrustumCommand(const u16 parViewId,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const mat4& parWorldViewTransform,
      const mat4& parProjectionMatrix,
      const bool parDrawFrustumNormals /*= false*/,
      const u32 parColor /*= 0xFFFFFFFF*/,
      const u32 parNormalsColor /*= 0xFFFFFFFF*/)
    : IDrawCommand(parViewId)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
    , FViewWorldTransform(parWorldViewTransform)
    , FInverseProjectionMatrix(Invert(parProjectionMatrix))
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
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_COLORS, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    FrustumCorners frustumCorners;
    frustumCorners.InitFromMatrices(FViewWorldTransform, FInverseProjectionMatrix);
    const MemoryView<const vec4> corners = frustumCorners.GetCorners();

    const u32 wantedVertices = 8 + ((FDrawFrustumNormals) ? 12 : 0);
    const u32 availableVertices = bgfx::getAvailTransientVertexBuffer(wantedVertices, layout);
    AssertRelease(availableVertices == wantedVertices);
    const u32 wantedIndices = 24 + ((FDrawFrustumNormals) ? 12 : 0);
    const u32 availableIndices = bgfx::getAvailTransientIndexBuffer(wantedIndices);
    AssertRelease(availableIndices == wantedIndices);

    bgfx::allocTransientBuffers(&vertexBuffer, layout, wantedVertices, &indexBuffer, wantedIndices);

    VertexDataStream stream(wantedVertices, hash.GetByteSize(), hash);

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, (corners[FrustumCorner::NEAR_BOTTOM_LEFT]).xyz());
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, (corners[FrustumCorner::NEAR_BOTTOM_RIGHT]).xyz());
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, (corners[FrustumCorner::FAR_BOTTOM_RIGHT]).xyz());
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, (corners[FrustumCorner::FAR_BOTTOM_LEFT]).xyz());
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, (corners[FrustumCorner::NEAR_TOP_LEFT]).xyz());
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, (corners[FrustumCorner::NEAR_TOP_RIGHT]).xyz());
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, (corners[FrustumCorner::FAR_TOP_RIGHT]).xyz());
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, (corners[FrustumCorner::FAR_TOP_LEFT]).xyz());
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FColor);
    stream.Advance();

    if (FDrawFrustumNormals)
    {
        Frustum f;
        f.InitFromCorners(frustumCorners);
        MemoryView<const vec4> planes = f.GetPlanes();

        // NearPlane
        const vec4 nearPlanePoint = (corners[FrustumCorner::NEAR_BOTTOM_LEFT] + corners[FrustumCorner::NEAR_BOTTOM_RIGHT] + corners[FrustumCorner::NEAR_TOP_LEFT] +
                                          corners[FrustumCorner::NEAR_TOP_RIGHT]) /
              4.f;
        PushNormalVertices(nearPlanePoint.xyz(), (planes[FrustumPlane::NEAR_PLANE]).xyz(), FNormalsColor, stream);

        // Farplane
        const vec4 farPlanePoint =
              (corners[FrustumCorner::FAR_BOTTOM_LEFT] + corners[FrustumCorner::FAR_BOTTOM_RIGHT] + corners[FrustumCorner::FAR_TOP_LEFT] + corners[FrustumCorner::FAR_TOP_RIGHT]) /
              4.f;
        PushNormalVertices(farPlanePoint.xyz(), (planes[FrustumPlane::FAR_PLANE]).xyz(), FNormalsColor, stream);

        // LEFTPlane
        const vec4 leftPlanePoint =
              (corners[FrustumCorner::NEAR_BOTTOM_LEFT] + corners[FrustumCorner::NEAR_TOP_LEFT] + corners[FrustumCorner::FAR_TOP_LEFT] + corners[FrustumCorner::FAR_BOTTOM_LEFT]) /
              4.f;
        PushNormalVertices(leftPlanePoint.xyz(), (planes[FrustumPlane::LEFT_PLANE]).xyz(), FNormalsColor, stream);

        // RightPlane
        const vec4 rightPlanePoint = (corners[FrustumCorner::NEAR_BOTTOM_RIGHT] + corners[FrustumCorner::NEAR_TOP_RIGHT] + corners[FrustumCorner::FAR_BOTTOM_RIGHT] +
                                           corners[FrustumCorner::FAR_TOP_RIGHT]) /
              4.f;
        PushNormalVertices(rightPlanePoint.xyz(), (planes[FrustumPlane::RIGHT_PLANE]).xyz(), FNormalsColor, stream);

        // TopPlane
        const vec4 topPlanePoint =
              (corners[FrustumCorner::NEAR_TOP_LEFT] + corners[FrustumCorner::NEAR_TOP_RIGHT] + corners[FrustumCorner::FAR_TOP_LEFT] + corners[FrustumCorner::FAR_TOP_RIGHT]) / 4.f;
        PushNormalVertices(topPlanePoint.xyz(), (planes[FrustumPlane::TOP_PLANE]).xyz(), FNormalsColor, stream);

        // BottomPlane
        const vec4 bottomPlanePoint = (corners[FrustumCorner::NEAR_BOTTOM_LEFT] + corners[FrustumCorner::NEAR_BOTTOM_RIGHT] + corners[FrustumCorner::FAR_BOTTOM_LEFT] +
                                            corners[FrustumCorner::FAR_BOTTOM_RIGHT]) /
              4.f;
        PushNormalVertices(bottomPlanePoint.xyz(), (planes[FrustumPlane::BOTTOM_PLANE]).xyz(), FNormalsColor, stream);
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

    mat4 transform = mat4::Identity();
    bgfx::setTransform(&transform.FValues[0]);

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
          const MultiPassMaterialInstanceHandle& parMultiPassMaterialInstanceHandle,
          const SkelettonPose* parSkelettonPose = nullptr,
          const mat4& parTransform = mat4::Identity());
    virtual ~DrawMeshCommand();

    virtual void SubmitCommand() const override;

private:
    mat4 FTransform;
    const SkelettonPose* FSkelettonPose = nullptr;
    MeshHandle FMeshHandle;
    MaterialInstanceHandle FMaterialInstanceHandle;
    MultiPassMaterialInstanceHandle FMultiPassMaterialInstanceHandle;
};

IMPLEMENT_POOL_ALLOCATED(DrawMeshCommand);
DrawMeshCommand::DrawMeshCommand(const u16 parViewId,
      const MeshHandle& parMeshHandle,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const MultiPassMaterialInstanceHandle& parMultiPassMaterialInstanceHandle,
      const SkelettonPose* parSkelettonPose /*= nullptr*/,
      const mat4& parTransform /*= mat4::Identity()*/)
    : IDrawCommand(parViewId)
    , FMeshHandle(parMeshHandle)
    , FMaterialInstanceHandle(parMaterialInstanceHandle)
    , FMultiPassMaterialInstanceHandle(parMultiPassMaterialInstanceHandle)
    , FTransform(parTransform)
    , FSkelettonPose(parSkelettonPose)
{
    AssertRelease(FMeshHandle.IsValid());
    AssertRelease(FMaterialInstanceHandle.IsValid() || FMultiPassMaterialInstanceHandle.IsValid());
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

    bgfx::setTransform(&FTransform.FValues[0]);

    bgfx::ProgramHandle program;
    if (FMaterialInstanceHandle.IsValid())
    {
        const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
        AssertRelease(instance != nullptr);

        instance->SetTextures();
        program = instance->GetProgram()->ProgramHandle();
    }
    else
    {
        const Rendering::MultiPassMaterialInstance* instance = Rendering::MaterialManager::GetMultiPassMaterialInstance(FMultiPassMaterialInstanceHandle);
        AssertRelease(instance != nullptr);

        instance->SetTextures();
        const Rendering::MultiPassProgram* multipassProgram = instance->GetProgram();
        AssertRelease(multipassProgram != nullptr && multipassProgram->HasSubstitution((Rendering::RenderPassId::Type)FViewId));

        program = multipassProgram->ProgramHandle((Rendering::RenderPassId::Type)FViewId);
    }

    AssertRelease(bgfx::isValid(program));

    if (FSkelettonPose != nullptr)
    {
        Rendering::MaterialManager::SetMat4Uniforms("u_skinningMatrices", FSkelettonPose->SkinningMatrices(), FSkelettonPose->BonesNumber());
    }

    RenderingState state;
    state.ApplyState();

    bgfx::submit(FViewId, program);
}

//----------------------------------------------------------------
//          DrawVerticesCommand
//----------------------------------------------------------------

class DrawVerticesCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(DrawVerticesCommand);

public:
    DrawVerticesCommand(const u16 parViewId,
          const vec3* parVertices,
          const u32 parVerticesSize,
          const u16* parIndices,
          const u32 parIndicesSize,
          const MaterialInstanceHandle& parMaterialInstanceHandle,
          const mat4& parTransform = mat4::Identity());
    virtual ~DrawVerticesCommand();

    virtual void SubmitCommand() const override;

private:
    const vec3* FVertices;
    const u32 FVerticesSize;
    const u16* FIndices;
    const u32 FIndicesSize;
    mat4 FTransform;
    MaterialInstanceHandle FMaterialInstanceHandle;
};

IMPLEMENT_POOL_ALLOCATED(DrawVerticesCommand);
DrawVerticesCommand::DrawVerticesCommand(const u16 parViewId,
      const vec3* parVertices,
      const u32 parVerticesSize,
      const u16* parIndices,
      const u32 parIndicesSize,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const mat4& parTransform /*= mat4::Identity()*/)
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
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION, true);
    hash.SetColorsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    u32 availableVertices = bgfx::getAvailTransientVertexBuffer(FVerticesSize, layout);
    AssertRelease(availableVertices == FVerticesSize);
    u32 availableIndices = bgfx::getAvailTransientIndexBuffer(FIndicesSize);
    AssertRelease(availableIndices == FIndicesSize);

    bgfx::allocTransientBuffers(&vertexBuffer, layout, FVerticesSize, &indexBuffer, FIndicesSize);

    VertexDataStream stream(FVerticesSize, hash.GetByteSize(), hash);

    forrange(i, 0, FVerticesSize)
    {
        stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, FVertices[i]);
        stream.Advance();
    }

    bx::memCopy(vertexBuffer.data, stream.GetData(), stream.GetByteSize());

    bx::memCopy(indexBuffer.data, FIndices, FIndicesSize * sizeof(u16));

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, FVerticesSize, vertexBuffer.layoutHandle);
    bgfx::setIndexBuffer(&indexBuffer, 0, FIndicesSize);

    bgfx::setTransform(&FTransform.FValues[0]);

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
          const mat4 parTransform = mat4::Identity());
    virtual ~DrawCircleCommand();

    virtual void SubmitCommand() const override;

private:
    CircleFeedbackParameters FParams;
    mat4 FTransform;
    MaterialInstanceHandle FMaterialInstanceHandle;
};

IMPLEMENT_POOL_ALLOCATED(DrawCircleCommand);

DrawCircleCommand::DrawCircleCommand(const u16 parViewId,
      const CircleFeedbackParameters& parParams,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const mat4 parTransform /*= mat4::Identity()*/)
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
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION, true);
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
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(-effectiveRange, 0.01f, -effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(effectiveRange, 0.01f, -effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(effectiveRange, 0.01f, effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(-effectiveRange, 0.01f, effectiveRange));
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

    bgfx::setTransform(&FTransform.FValues[0]);

    Rendering::RenderingState state(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_CULL_CW | BGFX_STATE_BLEND_ALPHA);
    state.ApplyState();

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
    AssertRelease(instance != nullptr);
    Rendering::MaterialManager::SetVec4Uniform("u_circleRadius", vec4(FParams.Range, FParams.Range - FParams.Thickness, 0.f, 0.f));

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
          const mat4 parTransform = mat4::Identity());
    virtual ~DrawCircularChunkCommand();

    virtual void SubmitCommand() const override;

private:
    CircularGridChunkFeedbackParameters FParameters;
    mat4 FTransform;
    MaterialInstanceHandle FMaterialInstanceHandle;
};

IMPLEMENT_POOL_ALLOCATED(DrawCircularChunkCommand);

DrawCircularChunkCommand::DrawCircularChunkCommand(const u16 parViewId,
      const CircularGridChunkFeedbackParameters& parParameters,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const mat4 parTransform /*= mat4::Identity()*/)
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
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION, true);
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
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(-effectiveRange, 0.01f, -effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FParameters.Color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(effectiveRange, 0.01f, -effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FParameters.Color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(effectiveRange, 0.01f, effectiveRange));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_COLORS, 0, FParameters.Color);
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(-effectiveRange, 0.01f, effectiveRange));
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

    bgfx::setTransform(&FTransform.FValues[0]);

    Rendering::RenderingState state(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_CULL_CW | BGFX_STATE_BLEND_ALPHA | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
    state.ApplyState();

    const Rendering::MaterialInstance* instance = Rendering::MaterialManager::GetMaterialInstance(FMaterialInstanceHandle);
    AssertRelease(instance != nullptr);
    Rendering::MaterialManager::SetVec4Uniform("u_innerOuterCircles",
          vec4(FParameters.InnerCircleRadius + 0.5f * FParameters.Thickness, FParameters.InnerCircleRadius, FParameters.OuterCircleRadius,
                FParameters.OuterCircleRadius - 0.5f * FParameters.Thickness));
    Rendering::MaterialManager::SetVec4Uniform("u_arcAngle", vec4(FParameters.ArcAngle));

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
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_POSITION, true);
    hash.SetValue(VERTEX_LAYOUT_PARAMS::HAS_UVS, true);
    hash.SetUVsNb(1);

    bgfx::VertexLayout layout = GetVertexLayout(hash);

    u32 availableVertices = bgfx::getAvailTransientVertexBuffer(4, layout);
    AssertRelease(availableVertices == 4);
    u32 availableIndices = bgfx::getAvailTransientIndexBuffer(6);
    AssertRelease(availableIndices == 6);

    bgfx::allocTransientBuffers(&vertexBuffer, layout, 4, &indexBuffer, 6);

    VertexDataStream stream(4, hash.GetByteSize(), hash);
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(-1.f, -1.f, 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, vec2(0.f, 1.f));
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(1.f, -1.f, 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, vec2(1.f, 1.f));
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(1.f, 1.f, 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, vec2(1.f, 0.f));
    stream.Advance();

    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, vec3(-1.f, 1.f, 0.f));
    stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, vec2(0.f, 0.f));
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
//          SetSamplerUniformCommand
//----------------------------------------------------------------

class SetSamplerUniformCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(SetSamplerUniformCommand);

public:
    SetSamplerUniformCommand(const u16 parViewId, const std::string& parUniformName, const TextureHandle& parHandle, const u32 parSlot);
    virtual ~SetSamplerUniformCommand() = default;

    virtual void SubmitCommand() const override;

private:
    std::string FUniformName;
    TextureHandle FHandle;
    u32 FSlot;
};

IMPLEMENT_POOL_ALLOCATED(SetSamplerUniformCommand);

SetSamplerUniformCommand::SetSamplerUniformCommand(const u16 parViewId, const std::string& parUniformName, const TextureHandle& parHandle, const u32 parSlot)
    : IDrawCommand(parViewId)
    , FUniformName(parUniformName)
    , FHandle(parHandle)
    , FSlot(parSlot)
{
}

void SetSamplerUniformCommand::SubmitCommand() const
{
    MaterialManager::SetSamplerUniform(FUniformName, FHandle, FSlot);
}

//----------------------------------------------------------------
//          SetFreeFormSamplerUniformCommand
//----------------------------------------------------------------

class SetFreeFormSamplerUniformCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(SetFreeFormSamplerUniformCommand);

public:
    SetFreeFormSamplerUniformCommand(const u16 parViewId, const std::string& parUniformName, const u32& parHandle, const u32 parSlot);
    virtual ~SetFreeFormSamplerUniformCommand() = default;

    virtual void SubmitCommand() const override;

private:
    std::string FUniformName;
    u32 FHandle;
    u32 FSlot;
};

IMPLEMENT_POOL_ALLOCATED(SetFreeFormSamplerUniformCommand);

SetFreeFormSamplerUniformCommand::SetFreeFormSamplerUniformCommand(const u16 parViewId, const std::string& parUniformName, const u32& parHandle, const u32 parSlot)
    : IDrawCommand(parViewId)
    , FUniformName(parUniformName)
    , FHandle(parHandle)
    , FSlot(parSlot)
{
}

void SetFreeFormSamplerUniformCommand::SubmitCommand() const
{
    MaterialManager::SetFreeFormSamplerUniform(FUniformName, FHandle, FSlot);
}

//----------------------------------------------------------------
//          SetVec4UniformCommand
//----------------------------------------------------------------

class SetVec4UniformCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(SetVec4UniformCommand);

public:
    SetVec4UniformCommand(const u16 parViewId, const std::string& parUniformName, const vec4& parUniformValue);
    virtual ~SetVec4UniformCommand() = default;

    virtual void SubmitCommand() const override;

private:
    std::string FUniformName;
    vec4 FData;
};

IMPLEMENT_POOL_ALLOCATED(SetVec4UniformCommand);

SetVec4UniformCommand::SetVec4UniformCommand(const u16 parViewId, const std::string& parUniformName, const vec4& parUniformValue)
    : IDrawCommand(parViewId)
    , FUniformName(parUniformName)
    , FData(parUniformValue)
{
}

void SetVec4UniformCommand::SubmitCommand() const
{
    MaterialManager::SetVec4Uniform(FUniformName, FData);
}

//----------------------------------------------------------------
//          SetMat3UniformCommand
//----------------------------------------------------------------

class SetMat3UniformCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(SetMat3UniformCommand);

public:
    SetMat3UniformCommand(const u16 parViewId, const std::string& parUniformName, const mat3& parUniformValue);
    virtual ~SetMat3UniformCommand() = default;

    virtual void SubmitCommand() const override;

private:
    std::string FUniformName;
    mat3 FData;
};

IMPLEMENT_POOL_ALLOCATED(SetMat3UniformCommand);

SetMat3UniformCommand::SetMat3UniformCommand(const u16 parViewId, const std::string& parUniformName, const mat3& parUniformValue)
    : IDrawCommand(parViewId)
    , FUniformName(parUniformName)
    , FData(parUniformValue)
{
}

void SetMat3UniformCommand::SubmitCommand() const
{
    MaterialManager::SetMat3Uniform(FUniformName, FData);
}

//----------------------------------------------------------------
//          SetMat4UniformCommand
//----------------------------------------------------------------

class SetMat4UniformCommand : public IDrawCommand
{
    DECLARE_POOL_ALLOCATED(SetMat4UniformCommand);

public:
    SetMat4UniformCommand(const u16 parViewId, const std::string& parUniformName, const mat4& parUniformValue);
    virtual ~SetMat4UniformCommand() = default;

    virtual void SubmitCommand() const override;

private:
    std::string FUniformName;
    mat4 FData;
};

IMPLEMENT_POOL_ALLOCATED(SetMat4UniformCommand);

SetMat4UniformCommand::SetMat4UniformCommand(const u16 parViewId, const std::string& parUniformName, const mat4& parUniformValue)
    : IDrawCommand(parViewId)
    , FUniformName(parUniformName)
    , FData(parUniformValue)
{
}

void SetMat4UniformCommand::SubmitCommand() const
{
    MaterialManager::SetMat4Uniform(FUniformName, FData);
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

void DrawCommandBuffer::SetViewTranform(const mat4& parViewTransform, const mat4& parProjection)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new SetViewTranformCommand(FViewId, parViewTransform, parProjection)));
}

void DrawCommandBuffer::DrawVertices(const vec3* parVertices,
      const u32 parVerticesSize,
      const u16* parIndices,
      const u32 parIndicesSize,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const mat4& parTransform)
{
    FCommandVector.push_back(
          std::unique_ptr<IDrawCommand>(new DrawVerticesCommand(FViewId, parVertices, parVerticesSize, parIndices, parIndicesSize, parMaterialInstanceHandle, parTransform)));
}

void DrawCommandBuffer::DrawMesh(const MeshHandle& parMeshHandle, const MaterialInstanceHandle& parMaterialInstanceHandle, const mat4& parTransform /*= mat4::Identity()*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(
          new DrawMeshCommand(FViewId, parMeshHandle, parMaterialInstanceHandle, Rendering::MultiPassMaterialInstanceHandle(), nullptr, parTransform)));
}

void DrawCommandBuffer::DrawMeshWithPose(const SkelettonPose* parSkelettonPose,
      const MeshHandle& parMeshHandle,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const mat4& parTransform /*= mat4::Identity()*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(
          new DrawMeshCommand(FViewId, parMeshHandle, parMaterialInstanceHandle, Rendering::MultiPassMaterialInstanceHandle(), parSkelettonPose, parTransform)));
}

void DrawCommandBuffer::DrawMesh(const MeshHandle& parMeshHandle, const MultiPassMaterialInstanceHandle& parMaterialInstanceHandle, const mat4& parTransform /*= mat4::Identity()*/)
{
    FCommandVector.push_back(
          std::unique_ptr<IDrawCommand>(new DrawMeshCommand(FViewId, parMeshHandle, Rendering::MaterialInstanceHandle(), parMaterialInstanceHandle, nullptr, parTransform)));
}

void DrawCommandBuffer::DrawMeshWithPose(const SkelettonPose* parSkelettonPose,
      const MeshHandle& parMeshHandle,
      const MultiPassMaterialInstanceHandle& parMaterialInstanceHandle,
      const mat4& parTransform /*= mat4::Identity()*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(
          new DrawMeshCommand(FViewId, parMeshHandle, Rendering::MaterialInstanceHandle(), parMaterialInstanceHandle, parSkelettonPose, parTransform)));
}

void DrawCommandBuffer::DrawAABB(const MaterialInstanceHandle& parMaterialInstanceHandle, const vec3& parMin, const vec3& parMax, const u32 parColor /*= 0xFFFFFFFF*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawAABBCommand(FViewId, parMaterialInstanceHandle, parMin, parMax, false, parColor)));
}

void DrawCommandBuffer::DrawAABBAsCube(const MaterialInstanceHandle& parMaterialInstanceHandle, const vec3& parMin, const vec3& parMax, const u32 parColor /*= 0xFFFFFFFF*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawAABBCommand(FViewId, parMaterialInstanceHandle, parMin, parMax, true, parColor)));
}

void DrawCommandBuffer::DrawFrustum(const MaterialInstanceHandle& parMaterialInstanceHandle,
      const mat4& parWorldViewTransform,
      const mat4& parProjectionMatrix,
      const bool parDrawFrustumNormals /*= false*/,
      const u32 parColor /*= 0xFFFFFFFF*/,
      const u32 parNormalsColor /*= 0xFFFFFFFF*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(
          new DrawFrustumCommand(FViewId, parMaterialInstanceHandle, parWorldViewTransform, parProjectionMatrix, parDrawFrustumNormals, parColor, parNormalsColor)));
}

void DrawCommandBuffer::DrawLines(const MaterialInstanceHandle& parMaterialInstanceHandle,
      const vec3* parVertices,
      const u32 parVerticesSize,
      const u32 parColor,
      const bool parClose)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawLines3DCommand(FViewId, parMaterialInstanceHandle, parVertices, parVerticesSize, parColor, parClose)));
}

void DrawCommandBuffer::DrawLines(const MaterialInstanceHandle& parMaterialInstanceHandle,
      const vec2* parVertices,
      const u32 parVerticesSize,
      const float parHeight,
      const u32 parColor,
      const bool parClose)
{
    FCommandVector.push_back(
          std::unique_ptr<IDrawCommand>(new DrawLines2DCommand(FViewId, parMaterialInstanceHandle, parVertices, parVerticesSize, parHeight, parColor, parClose)));
}

void DrawCommandBuffer::DrawLines(const MaterialInstanceHandle& parMaterialInstanceHandle,
      const std::vector<vec2>& parVertices,
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
      const mat4& parTransform /*= mat4::Identity()*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawCircleCommand(FViewId, parParams, parMaterialInstanceHandle, parTransform)));
}

void DrawCommandBuffer::DrawCircularChunk(const CircularGridChunkFeedbackParameters& parParameters,
      const MaterialInstanceHandle& parMaterialInstanceHandle,
      const mat4 parTransform /*= mat4::Identity()*/)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new DrawCircularChunkCommand(FViewId, parParameters, parMaterialInstanceHandle, parTransform)));
}

void DrawCommandBuffer::BlitWithMaterial(const MaterialInstanceHandle& parMaterialInstanceHandle)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new BlitWithMaterialCommand(FViewId, parMaterialInstanceHandle)));
}

void DrawCommandBuffer::SetSamplerUniform(const std::string& parUniformName, const TextureHandle& parHandle, const u32 parSlot)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new SetSamplerUniformCommand(FViewId, parUniformName, parHandle, parSlot)));
}

void DrawCommandBuffer::SetFreeFormSamplerUniform(const std::string& parUniformName, const u32& parHandle, const u32 parSlot)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new SetFreeFormSamplerUniformCommand(FViewId, parUniformName, parHandle, parSlot)));
}

void DrawCommandBuffer::SetVec4Uniform(const std::string& parUniformName, const vec4& parUniformValue)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new SetVec4UniformCommand(FViewId, parUniformName, parUniformValue)));
}

void DrawCommandBuffer::SetMat3Uniform(const std::string& parUniformName, const mat3& parUniformValue)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new SetMat3UniformCommand(FViewId, parUniformName, parUniformValue)));
}

void DrawCommandBuffer::SetMat4Uniform(const std::string& parUniformName, const mat4& parUniformValue)
{
    FCommandVector.push_back(std::unique_ptr<IDrawCommand>(new SetMat4UniformCommand(FViewId, parUniformName, parUniformValue)));
}

void DrawCommandBuffer::Submit()
{
    foreachitem(command, FCommandVector)
    {
        command->SubmitCommand();
    }
}

} // namespace Rendering
} // namespace ECSEngine
