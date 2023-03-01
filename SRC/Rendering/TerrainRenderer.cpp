#include "stdafx.h"

#include "TerrainRenderer.h"

#include "Common/CameraManager.h"
#include "Common/ColorUtils.h"
#include "Common/MeshStreamingData.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshManager.h"

namespace ECSEngine
{
namespace Rendering
{

vec4 GetLoDColor(u32 LoDLevel)
{
    switch (LoDLevel)
    {
    case 0:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(255, 255, 255, 255));
        break;
    case 1:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(255, 0, 255, 255));
        break;
    case 2:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(255, 255, 0, 255));
        break;
    case 3:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(0, 0, 255, 255));
        break;
    case 4:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(0, 255, 0, 255));
        break;
    case 5:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(255, 0, 0, 255));
        break;
    default:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(255, 255, 255, 255));
        break;
    }
}

void TerrainRenderer::Initialize()
{
    CommandBuffer = BGFXRenderingBackend::Instance().CreateCommandBuffer(RenderPassId::EDITOR_PASS);

    CreateTerrainMesh();

    for (int i = 0; i < FTerrainDescriptor.NumberLoDLevels; i++)
    {
        FTerrainDescriptor.LoDDistances.push_back(FTerrainDescriptor.MinLodDistance * powf(2.f, i));
    }

    FQuadTree.Initialize(&FTerrainDescriptor);
}

void TerrainRenderer::Shutdown()
{
    // Release terrain mesh
    MeshManager::Instance().ReleaseMesh(FTerrainMesh);

    // release command buffer
    CommandBuffer->clear();

    BGFXRenderingBackend::Instance().ReleaseCommandBuffer(CommandBuffer);
    CommandBuffer = nullptr;
}

void TerrainRenderer::Render()
{
    SCOPED_PROFILE_CLASS(TerrainRenderer, Render);
    u32 FCameraId = CameraManager::Instance().CreateCameraIFN("EditorCamera");
    Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
    AssertRelease(camera != nullptr);

    const vec3 at(500.0f, 0.0f, 500.0f);
    const vec3 eye(0.0f, 100.0f, -10.0f);

    mat4 worldWiewMatrix = LookAt(eye, at, vec3(0, 1.0f, 0.0f));

    Camera c;
    c.Init(worldWiewMatrix, Radians(60.0f), 1.f, 1500.0f);

    std::vector<QuadTreeNode> nodesToRender;
    FQuadTree.FillRegionsToRender(*camera, nodesToRender);

    CommandBuffer->clear();
    const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();
    CommandBuffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(aspectRatio));

    MultiPassMaterialInstanceHandle handle = MaterialManager::CreateMultiPassMaterialInstanceIFN("materials\\terrain.materialv2");

    std::vector<vec4> LoDs;
    u32 nbLods = FTerrainDescriptor.LoDDistances.size();
    for (u32 i = 0; i < nbLods; i += 4)
    {
        vec4 res;
        res.x = FTerrainDescriptor.LoDDistances[i];
        if ((i + 1) < nbLods)
            res.y = FTerrainDescriptor.LoDDistances[i + 1];
        if ((i + 2) < nbLods)
            res.z = FTerrainDescriptor.LoDDistances[i + 2];
        if ((i + 3) < nbLods)
            res.w = FTerrainDescriptor.LoDDistances[i + 3];

        LoDs.push_back(res);
    }

    MaterialManager::SetVec4Uniforms("u_LoDDistances", LoDs.data(), (u32)LoDs.size());
    CommandBuffer->SetVec4Uniform("u_gridDim", vec4(FTerrainDescriptor.MeshVerticesSize, FTerrainDescriptor.MeshVerticesSize, 0.f, 0.f));
    /*CommandBuffer->SetMat4Uniform("u_debugViewMat", c.GetWorldViewMatrix());*/

    forrange(i, 0, nodesToRender.size())
    {
        CommandBuffer->SetVec4Uniform("u_bboxMinAndMaxXY", vec4(nodesToRender[i].BBox.Min().xz(), nodesToRender[i].BBox.Max().xz()));
        CommandBuffer->SetVec4Uniform("u_LoDColor", GetLoDColor(nodesToRender[i].LoDLevel));
        CommandBuffer->SetVec4Uniform("u_currentLoD", vec4(nodesToRender[i].LoDLevel));
        CommandBuffer->DrawMesh(FTerrainMesh, handle);
    }

    CommandBuffer->Submit();
}

void TerrainRenderer::CreateTerrainMesh()
{
    const VertexLayoutHash vertexHash(true, 0, 1, 0, 0, 0, 0);

    const u32 nbVertices = FTerrainDescriptor.MeshVerticesSize * FTerrainDescriptor.MeshVerticesSize;

    const u32 terrainSizeM1 = FTerrainDescriptor.MeshVerticesSize - 1;
    const u32 nbTriangles = terrainSizeM1 * terrainSizeM1 * 2;

    // create vertices
    VertexDataStream stream(nbVertices, vertexHash.GetByteSize(), vertexHash, false);
    forrange(i, 0, FTerrainDescriptor.MeshVerticesSize)
    {
        forrange(j, 0, FTerrainDescriptor.MeshVerticesSize)
        {
            const vec3 pos((float)i / (float)terrainSizeM1, 0.f, (float)j / (float)terrainSizeM1);
            stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSTION, 0, pos);
            stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_UVS, 0, pos.xz());
            stream.Advance();
        }
    }

    // add indices
    std::vector<u32> indices;
    indices.reserve(nbTriangles * 3);
    forrange(i, 0, terrainSizeM1)
    {
        forrange(j, 0, terrainSizeM1)
        {
            u32 idx0 = i * FTerrainDescriptor.MeshVerticesSize + j;
            u32 idx1 = i * FTerrainDescriptor.MeshVerticesSize + j + 1;
            u32 idx2 = (i + 1) * FTerrainDescriptor.MeshVerticesSize + j + 1;
            u32 idx3 = (i + 1) * FTerrainDescriptor.MeshVerticesSize + j;

            indices.push_back(idx0);
            indices.push_back(idx2);
            indices.push_back(idx1);

            indices.push_back(idx0);
            indices.push_back(idx3);
            indices.push_back(idx2);
        }
    }

    FTerrainMesh = MeshManager::Instance().CreateMesh(stream, indices.data(), indices.size() * sizeof(u32));
}

} // namespace Rendering
} // namespace ECSEngine
