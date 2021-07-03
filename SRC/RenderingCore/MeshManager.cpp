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
    MeshHandle handle = GetNextHandle();
    Mesh* mesh = MeshHelpers::CreateIMesh(parVertexData.GetHash());

    mesh->SetRawVertexData(parVertexData);
    mesh->SetRawIndexData(parIndexData, parIndexDataSizeInBytes);

    FMeshes[handle] = mesh;

    return handle;
}

const MeshHandle MeshManager::CreateMesh(const std::string& parFilename)
{
    Resource res(parFilename);
    return CreateMesh(res);
}

const MeshHandle MeshManager::CreateMesh(const Resource& parResource)
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

        MeshHandle handle = GetNextHandle();
        reader.ReadMesh(handle, mesh, parResource);
        AssertRelease(mesh != nullptr);
        FMeshes[handle] = mesh;
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

void MeshManager::ReleaseMesh(MeshHandle parHandle)
{
    if (parHandle.GetMeshId() >= FMeshes.size())
        return;
    if (FMeshes[parHandle] == nullptr)
        return;

    delete FMeshes[parHandle];
    FMeshes[parHandle] = nullptr;

    for (auto it = FFileToMesh.begin(); it != FFileToMesh.end(); ++it)
    {
        if (it->second == parHandle)
        {
            FFileToMesh.erase(it);
            return;
        }
    }
}

MeshHandle MeshManager::GetNextHandle()
{
    forrange(i, 0, FMeshes.size())
    {
        if (FMeshes[i] == nullptr)
            return MeshHandle(i);
    }
    MeshHandle handle(FMeshes.size());
    FMeshes.push_back(nullptr);
    return handle;
}

} // namespace Rendering
} // namespace ECSEngine
