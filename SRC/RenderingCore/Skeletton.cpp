#include "stdafx.h"

#include "Skeletton.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(SkelettonJoint);
IMPLEMENT_POOL_ALLOCATED(Skeletton);
IMPLEMENT_POOL_ALLOCATED(SkelettonPose);

Skeletton::~Skeletton()
{
    delete[] FJoints;
}

void Skeletton::SetBonesNumber(const u8 parBonesNumber)
{
    AssertRelease(FJoints == nullptr);
    AssertReleaseMsg(FBonesNumber == 0, "Please, do not resize bones...");
    FJoints = new SkelettonJoint[parBonesNumber];
    FBonesNumber = parBonesNumber;
}

SkelettonJoint& Skeletton::GetJoint(u8 parIndex)
{
    AssertRelease(parIndex < FBonesNumber);
    return FJoints[parIndex];
}

const SkelettonJoint& Skeletton::GetJoint(u8 parIndex) const
{
    AssertRelease(parIndex < FBonesNumber);
    return FJoints[parIndex];
}

u8 Skeletton::FindSkelettonJoint(const char* parName) const
{
    forrange(i, 0, FBonesNumber)
    {
        if (strcmp(FJoints[i].Name, parName) == 0)
            return i;
    }
    return -1;
}

SkelettonPose::~SkelettonPose()
{
    delete[] FLocalPoses;
    delete[] FSkinningMatrix;
}

void SkelettonPose::InitialiseFromSkeletton()
{
    // TODO init local poses by inverting local poses in the skeletton
    AssertRelease(FSkeletton != nullptr);
    const u8 nbBones = FSkeletton->NumberOfBones();
    FLocalPoses = new glm::mat4[nbBones];
    FSkinningMatrix = new glm::mat4[nbBones];

    forrange(i, 0, nbBones) { FLocalPoses[i] = glm::inverse(FSkeletton->GetJoint(i).InvBindPose); }

    // Set dirty
    SetDirty();

    // call the update global pose
    UpdateSkinningMatrix();
}

void SkelettonPose::UpdateSkinningMatrix()
{
    // TODO FineGrain dirty hack ?
    if (!FDirty)
        return;

    // multiply matrices alltogether to get the global pose
    const u8 nbBones = FSkeletton->NumberOfBones();
    std::vector<bool> computedGlobalMatrices(nbBones, false);

    forrange(i, 0, nbBones) { ComputeSkinningMatrixForJoint(i, computedGlobalMatrices); }

    // multiply by the joint invert bind pose
    forrange(i, 0, nbBones)
    {
        const SkelettonJoint& joint = FSkeletton->GetJoint(i);
        FSkinningMatrix[i] = FSkinningMatrix[i] * joint.ModelToJointMatrix;
    }

    // in the shader, push the skinning array into u_skinningMatrices
}

void SkelettonPose::SetDirty()
{
    FDirty = true;
}

void SkelettonPose::ComputeSkinningMatrixForJoint(const u8 parIndex, std::vector<bool>& parComputedCache)
{
    // early bail
    if (parComputedCache[parIndex])
        return;

    const SkelettonJoint& joint = FSkeletton->GetJoint(parIndex);

    // si on est parent, alors on prend la local pose
    if (joint.ParentId == 0xFF)
    {
        FSkinningMatrix[parIndex] = FLocalPoses[parIndex];
        parComputedCache[parIndex] = true;
        return;
    }

    // si le parent est pas compute, alors on recurse (beurk)
    if (!parComputedCache[joint.ParentId])
    {
        ComputeSkinningMatrixForJoint(joint.ParentId, parComputedCache);
    }

    FSkinningMatrix[parIndex] = FLocalPoses[parIndex] * FSkinningMatrix[joint.ParentId];
    parComputedCache[parIndex] = true;
}

} // namespace Rendering
} // namespace ECSEngine
