#pragma once
#include "Common/RenderingHandles.h"
#include "Common/Singleton.h"

namespace ECSEngine
{
class Resource;
namespace Rendering
{
class VertexDataStream;
class Mesh;
class MeshManager : public Singleton<MeshManager>
{
public:
    MeshManager();
    ~MeshManager();

    const MeshHandle CreateMesh(const std::string& parFilename);
    const MeshHandle CreateMesh(const Resource& parResource);
    const MeshHandle CreateMesh(const VertexDataStream& parVertexData, const void* parIndexData, const u32 parIndexDataSizeInBytes);

    Mesh* GetMesh(const MeshHandle& meshHandle) const;

    void ReleaseMesh(MeshHandle parHandle);

private:
    MeshHandle GetNextHandle();

private:
    std::vector<Mesh*> FMeshes;
    std::map<std::string, u32> FFileToMesh;
};
} // namespace Rendering
} // namespace ECSEngine
