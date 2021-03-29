#include "stdafx.h"

#include "Skeletton.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(SkelettonJoint);

Skeletton::~Skeletton()
{
    delete[] FJoints;
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

} // namespace Rendering
} // namespace ECSEngine
