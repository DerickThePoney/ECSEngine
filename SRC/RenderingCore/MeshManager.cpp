#include "stdafx.h"

#include "MeshManager.h"

#include "Mesh.h"
#include "VertexLayout.h"

namespace ECSEngine
{
namespace Rendering
{
namespace MeshHelpers
{
template<typename VertexLayout>
IMesh* CreateIMesh(const VertexLayout& parLayout)
{
    return new Mesh<VertexLayout>();
}
} // namespace MeshHelpers

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

const MeshHandle MeshManager::CreateMesh(const void* parVertexData, const u32 parVertexDataSizeInBytes, const void* parIndexData, const u32 parIndexDataSizeInBytes)
{
    MeshHandle handle((u32)FMeshes.size());
    using VertexLayout = ECSEngine::Rendering::VertexPositionColorN<1>;

    IMesh* mesh = MeshHelpers::CreateIMesh(VertexLayout());

    mesh->SetRawVertexData(parVertexData, parVertexDataSizeInBytes, true);
    mesh->SetRawIndexData(parIndexData, parIndexDataSizeInBytes, true);

    FMeshes.push_back(mesh);

    return handle;
}

const MeshHandle MeshManager::CreateMesh(const std::string& parFilename)
{
    AssertNotReached();
    return MeshHandle();
}

IMesh* MeshManager::GetMesh(const MeshHandle& meshHandle) const
{
    AssertRelease(meshHandle.IsValid());
    AssertRelease(meshHandle.GetMeshId() < FMeshes.size());
    AssertRelease(FMeshes[meshHandle.GetMeshId()] != nullptr);
    return FMeshes[meshHandle.GetMeshId()];
}

} // namespace Rendering
} // namespace ECSEngine