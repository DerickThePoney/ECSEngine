#pragma once
#include "Common/RenderingHandles.h"
#include "Common/Singleton.h"
#include "Skeletton.h"

namespace ECSEngine
{
namespace Rendering
{
class SkelettonPoseManager : public Singleton<SkelettonPoseManager>
{
public:
    Skeletton* CreateSkeletton_ReturnCreatedSkeletton(const MeshHandle parMesh);

    const Skeletton* GetSkeletton(const MeshHandle parMesh);
    SkelettonPose* GetPose(const MeshHandle parMesh);

    void UpdateSkinningMatrices();

private:
    std::map<MeshHandle, Skeletton> FSkelettons;
    std::map<MeshHandle, u32> FMeshToPoseMap;
    std::vector<SkelettonPose> FSkelettonPoses;
};
} // namespace Rendering
} // namespace ECSEngine
