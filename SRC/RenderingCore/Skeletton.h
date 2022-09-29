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
    mat4 InvBindPose = mat4::Identity();
    mat4 ModelToJointMatrix = mat4::Identity();
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
    mat4* LocalPoses() { return FLocalPoses; }
    const mat4* SkinningMatrices() const { return FSkinningMatrices; }

    u8 BonesNumber() const
    {
        AssertRelease(FSkeletton != nullptr);
        return FSkeletton->NumberOfBones();
    }

    void SetDirty();

private:
    void ComputeSkinningMatrixForJoint(const u8 parIndex, std::vector<bool>& parComputedCache);

private:
    const Skeletton* FSkeletton = nullptr;
    mat4* FLocalPoses = nullptr;
    mat4* FSkinningMatrices = nullptr;
    bool FDirty = false;
};
} // namespace Rendering
} // namespace ECSEngine
