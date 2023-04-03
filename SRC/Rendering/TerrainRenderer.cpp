#include "stdafx.h"

#include "TerrainRenderer.h"

#include "Common/CameraManager.h"
#include "Common/ColorUtils.h"
#include "Common/MeshStreamingData.h"
#include "Common/RenderingHandles.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "HeightMap.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/Material.h"
#include "RenderingCore/MaterialDescriptors.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshManager.h"
#include "RenderingCore/Texture.h"
#include "RenderingCore/TextureDescriptor.h"
#include "RenderingCore/TexturesManager.h"
#include "RenderingCore/VertexBuffer.h"

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
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(0, 255, 255, 255));
        break;
    case 4:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(0, 0, 255, 255));
        break;
    case 5:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(0, 255, 0, 255));
        break;
    case 6:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(255, 0, 0, 255));
        break;
    case 7:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(150, 150, 150, 255));
        break;
    case 8:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(150, 0, 150, 255));
        break;
    case 9:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(150, 150, 0, 255));
        break;
    case 10:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(0, 150, 150, 255));
        break;
    case 11:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(60, 60, 60, 255));
        break;
    default:
        return ColorUtils::ConvertToFVEC4(ColorUtils::FromRGBA(255, 10, 255, 255));
        break;
    }
}

TerrainRenderer::TerrainRenderer()
{
}

TerrainRenderer::~TerrainRenderer()
{
}

void TerrainRenderer::Initialize()
{
    CommandBuffer = BGFXRenderingBackend::Instance().CreateCommandBuffer(RenderPassId::EDITOR_PASS);

    CreateTerrainMesh();

    for (int i = 0; i < FTerrainDescriptor.NumberLoDLevels; i++)
    {
        FTerrainDescriptor.LoDDistances.push_back(FTerrainDescriptor.MinLodDistance * powf(2.f, i));
    }

    MultiPassMaterialInstanceHandle handle = MaterialManager::CreateMultiPassMaterialInstanceIFN("materials\\terrain.materialv2");
    const MultiPassMaterialInstance* materialInstance = MaterialManager::GetMultiPassMaterialInstance(handle);
    const TextureHandle& heightMapHandle = materialInstance->GetTextureInputs()[0].Handle();

    HeightMap::CreateIFP();
    HeightMap::Instance().Initialise(FTerrainDescriptor, heightMapHandle);

    FQuadTree.Initialize(&FTerrainDescriptor);
}

void TerrainRenderer::Shutdown()
{
    HeightMap::Instance().Shutdown();
    HeightMap::Destroy();

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

    std::vector<QuadTreeNode> nodesToRender;
    FQuadTree.FillRegionsToRender(*camera, nodesToRender);

    if (nodesToRender.empty())
    {
        return;
    }

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
    CommandBuffer->SetVec4Uniform(
          "u_gridDim", vec4(FTerrainDescriptor.MeshVerticesSize, FTerrainDescriptor.MeshVerticesSize, FTerrainDescriptor.TerrainSize, FTerrainDescriptor.TerrainSize));

    constexpr u32 stride = sizeof(vec4) + sizeof(vec4) + sizeof(vec4);
    const u32 nbNodes = (u32)nodesToRender.size();
    // figure out how big of a buffer is available
    if (nbNodes > 0)
    {
        const u32 nodesToDrawInstanced = bgfx::getAvailInstanceDataBuffer(nbNodes, stride);
        if (nodesToDrawInstanced > 0)
        {
            bgfx::InstanceDataBuffer idb;
            bgfx::allocInstanceDataBuffer(&idb, nbNodes, stride);
            u8* data = idb.data;
            forrange(i, 0, nodesToRender.size())
            {
                vec4* dataAsVec4 = reinterpret_cast<vec4*>(data);
                dataAsVec4[0] = vec4(nodesToRender[i].BBox.Min().xz(), nodesToRender[i].BBox.Max().xz());
                dataAsVec4[1] = GetLoDColor(nodesToRender[i].LoDLevel);
                dataAsVec4[2] = vec4((float)nodesToRender[i].LoDLevel);
                data += stride;
            }
            bgfx::setInstanceDataBuffer(&idb);
            CommandBuffer->DrawMesh(FTerrainMesh, handle);
            CommandBuffer->Submit();
        }
    }
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
            stream.PushData(VERTEX_LAYOUT_PARAMS::HAS_POSITION, 0, pos);
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
