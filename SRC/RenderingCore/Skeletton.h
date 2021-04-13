#pragma once
#include "Common/PoolAllocator.h"

namespace ECSEngine
{
namespace Rendering
{
struct SkelettonJoint
{
    DECLARE_POOL_ALLOCATED(SkelettonJoint);

public:
    glm::mat4 InvBindPose = glm::identity<glm::mat4>();
    glm::mat4 ModelToJointMatrix = glm::identity<glm::mat4>();
    std::string Name;
    u8 ParentId = 0xFF;
};

class Skeletton
{
    DECLARE_POOL_ALLOCATED(Skeletton);

public:
    ~Skeletton();

    void SetBonesNumber(const u8 parBonesNumber);

    u8 NumberOfBones() const { return FBonesNumber; }
    SkelettonJoint& GetJoint(u8 parIndex);
    const SkelettonJoint& GetJoint(u8 parIndex) const;

    u8 FindSkelettonJoint(const std::string& parName) const;

private:
    u8 FBonesNumber = 0;
    SkelettonJoint* FJoints = nullptr;
};

class SkelettonPose
{
    DECLARE_POOL_ALLOCATED(SkelettonPose);

public:
    SkelettonPose(const Skeletton* parSkeletton)
        : FSkeletton(parSkeletton)
    {
        AssertRelease(FSkeletton != nullptr);
    }

    ~SkelettonPose();

    void InitialiseFromSkeletton();
    void UpdateSkinningMatrix();

    const Skeletton* GetSkeletton() const { return FSkeletton; }
    glm::mat4* LocalPoses() { return FLocalPoses; }
    const glm::mat4* SkinningMatrix() const { return FSkinningMatrix; }

    void SetDirty();

private:
    void ComputeSkinningMatrixForJoint(const u8 parIndex, std::vector<bool>& parComputedCache);

private:
    const Skeletton* FSkeletton = nullptr;
    glm::mat4* FLocalPoses = nullptr;
    glm::mat4* FSkinningMatrix = nullptr;
    bool FDirty = false;
};
} // namespace Rendering
} // namespace ECSEngine
