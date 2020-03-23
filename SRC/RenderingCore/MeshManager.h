#pragma once
#include "Common/MeshHandle.h"
#include "Common/Singleton.h"

namespace ECSEngine
{
class Resource;
namespace Rendering
{

class IMesh;
class MeshManager : public Singleton<MeshManager>
{
public:
    MeshManager();
    ~MeshManager();

    const MeshHandle CreateMesh(const std::string& parFilename);
    const MeshHandle CreateMesh(Resource& parResource);
    const MeshHandle CreateMesh(const void* parVertexData, const u32 parVertexDataSizeInBytes, const void* parIndexData, const u32 parIndexDataSizeInBytes);

    IMesh* GetMesh(const MeshHandle& meshHandle) const;

private:
    std::vector<IMesh*> FMeshes;
    std::map<std::string, u32> FFileToMesh;
};
} // namespace Rendering
} // namespace ECSEngine
