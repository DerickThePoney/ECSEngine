#pragma once
#include "Common/RenderingHandles.h"
#include "Common/Singleton.h"
#include "Skeletton.h"

namespace ECSEngine
{
namespace Rendering
{
class SkelettonManager : public Singleton<SkelettonManager>
{
public:
    Skeletton* CreateSkeletton_ReturnCreatedSkeletton(const MeshHandle parMesh);

    SkelettonPose* CreateSkelettonPose_ReturnPose(const u32 parGFXId, const MeshHandle parMesh);

    const Skeletton* GetSkeletton(const MeshHandle parMesh);
    SkelettonPose* GetPose(const u32 parGFXId);
    void DeleteSkelettonPose(const u32 parGFXId);

    void UpdateSkinningMatrices();

private:
    std::map<MeshHandle, Skeletton> FSkelettons;
    std::map<u32, u32> FGFXToSquelettonPose;
    std::vector<SkelettonPose> FSkelettonPoses;
};
} // namespace Rendering
} // namespace ECSEngine
