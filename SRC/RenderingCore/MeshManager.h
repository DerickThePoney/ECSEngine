#pragma once
#include "Common/MeshHandle.h"
#include "Common/Singleton.h"

namespace ECSEngine
{
namespace Rendering
{

class IMesh;
class MeshManager : public Singleton<MeshManager>
{
public:
    MeshManager();
    ~MeshManager();

    const MeshHandle CreateMesh(const std::string& parFilename);
    const MeshHandle CreateMesh(const void* parVertexData, const u32 parVertexDataSizeInBytes, const void* parIndexData, const u32 parIndexDataSizeInBytes);

    IMesh* GetMesh(const MeshHandle& meshHandle) const;

private:
    std::vector<IMesh*> FMeshes;
};
} // namespace Rendering
} // namespace ECSEngine
