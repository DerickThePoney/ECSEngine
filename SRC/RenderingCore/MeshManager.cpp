#include "stdafx.h"

#include "MeshManager.h"

#include "Common/MeshStreamingData.h"
#include "Common/Resource.h"
#include "Mesh.h"
#include "MeshFileReader.h"
#include "MeshUtils.h"
#include "VertexLayout.h"

namespace ECSEngine
{
namespace Rendering
{

MeshManager::MeshManager()
    : Singleton()
{
}

MeshManager::~MeshManager()
{
    foreachitem(mesh, FMeshes)
    {
        delete mesh;
        mesh = nullptr;
    }
}

const MeshHandle MeshManager::CreateMesh(const VertexDataStream& parVertexData, const void* parIndexData, const u32 parIndexDataSizeInBytes)
{
    MeshHandle handle((u32)FMeshes.size());
    Mesh* mesh = MeshHelpers::CreateIMesh(parVertexData.GetHash());

    mesh->SetRawVertexData(parVertexData);
    mesh->SetRawIndexData(parIndexData, parIndexDataSizeInBytes);

    FMeshes.push_back(mesh);

    return handle;
}

const MeshHandle MeshManager::CreateMesh(const std::string& parFilename)
{
    Resource res(parFilename);
    return CreateMesh(res);
}

const MeshHandle MeshManager::CreateMesh(Resource& parResource)
{
    auto itFind = FFileToMesh.find(parResource.FName);

    if (itFind != FFileToMesh.end())
    {
        return MeshHandle(itFind->second);
    }
    else
    {
        MeshFileReader reader;
        Mesh* mesh = nullptr;

        reader.ReadMesh(mesh, parResource);

        AssertRelease(mesh != nullptr);
        MeshHandle handle((u32)FMeshes.size());
        FMeshes.push_back(mesh);
        FFileToMesh[parResource.FName] = handle.GetMeshId();
        return handle;
    }
}

Mesh* MeshManager::GetMesh(const MeshHandle& meshHandle) const
{
    AssertRelease(meshHandle.IsValid());
    AssertRelease(meshHandle.GetMeshId() < FMeshes.size());
    AssertRelease(FMeshes[meshHandle.GetMeshId()] != nullptr);
    return FMeshes[meshHandle.GetMeshId()];
}

} // namespace Rendering
} // namespace ECSEngine
