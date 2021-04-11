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

    // Set dirty

    // call the update global pose
}

void SkelettonPose::UpdateSkinningMatrix()
{
    // multiply matrices alltogether to get the global pose

    // multiply by the joint invert bind pose

    // in the shader, push the skinning array into u_skinningMatrices
}

void SkelettonPose::SetDirty()
{
    FDirty = true;
}

} // namespace Rendering
} // namespace ECSEngine
