#include "stdafx.h"

#include "SkelettonManager.h"

namespace ECSEngine
{
namespace Rendering
{

Skeletton* SkelettonManager::CreateSkeletton_ReturnCreatedSkeletton(const MeshHandle parMesh)
{
    AssertRelease(FSkelettons.find(parMesh) == FSkelettons.end());
    auto it = FSkelettons.insert_or_assign(parMesh, Skeletton());
    return &it.first->second;
}

SkelettonPose* SkelettonManager::CreateSkelettonPose_ReturnPose(const u32 parGFXId, const MeshHandle parMesh)
{
    AssertRelease(FGFXToSquelettonPose.find(parGFXId) == FGFXToSquelettonPose.end());
    auto itSqueletton = FSkelettons.find(parMesh);
    if (itSqueletton == FSkelettons.end())
        return nullptr;

    u32 poseId = (u32)FSkelettonPoses.size();
    FSkelettonPoses.push_back(SkelettonPose(&itSqueletton->second));
    FGFXToSquelettonPose.insert_or_assign(parGFXId, poseId);
    return &FSkelettonPoses[poseId];
}

const Skeletton* SkelettonManager::GetSkeletton(const MeshHandle parMesh)
{
    auto itFind = FSkelettons.find(parMesh);
    if (itFind == FSkelettons.end())
        return nullptr;

    return &itFind->second;
}

SkelettonPose* SkelettonManager::GetPose(const u32 parGFXId)
{
    auto itFind = FGFXToSquelettonPose.find(parGFXId);
    if (itFind == FGFXToSquelettonPose.end())
        return nullptr;

    AssertRelease(FSkelettonPoses.size() > itFind->second);
    return &FSkelettonPoses[itFind->second];
}

void SkelettonManager::UpdateSkinningMatrices()
{
    foreachitem(pose, FSkelettonPoses) { pose.UpdateSkinningMatrix(); }
}

} // namespace Rendering
} // namespace ECSEngine