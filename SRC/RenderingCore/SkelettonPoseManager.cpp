#include "stdafx.h"

#include "SkelettonPoseManager.h"

namespace ECSEngine
{
namespace Rendering
{

Skeletton* SkelettonPoseManager::CreateSkeletton_ReturnCreatedSkeletton(const MeshHandle parMesh)
{
    AssertRelease(FSkelettons.find(parMesh) == FSkelettons.end());
    auto it = FSkelettons.insert_or_assign(parMesh, Skeletton());
    FMeshToPoseMap.insert_or_assign(parMesh, (u32)FSkelettonPoses.size());

    FSkelettonPoses.push_back(SkelettonPose(&it.first->second));
    return &it.first->second;
}

const Skeletton* SkelettonPoseManager::GetSkeletton(const MeshHandle parMesh)
{
    auto itFind = FSkelettons.find(parMesh);
    if (itFind == FSkelettons.end())
        return nullptr;

    return &itFind->second;
}

SkelettonPose* SkelettonPoseManager::GetPose(const MeshHandle parMesh)
{
    auto itFind = FMeshToPoseMap.find(parMesh);
    if (itFind == FMeshToPoseMap.end())
        return nullptr;

    AssertRelease(FSkelettonPoses.size() > itFind->second);
    return &FSkelettonPoses[itFind->second];
}

void SkelettonPoseManager::UpdateSkinningMatrices()
{
    foreachitem(pose, FSkelettonPoses) { pose.UpdateSkinningMatrix(); }
}

} // namespace Rendering
} // namespace ECSEngine