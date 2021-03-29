#pragma once
#include "Common/PoolAllocator.h"

namespace ECSEngine
{
namespace Rendering
{
struct SkelettonJoint
{
    DECLARE_POOL_ALLOCATED(SkelettonJoint);

    glm::mat4 InvBindPose = glm::identity<glm::mat4>();
    const char* Name = nullptr;
    const u8 ParentId = 0xFF;
};

class Skeletton
{
public:
    ~Skeletton();

    u8 NumberOfBones() const { return FBonesNumber; }
    SkelettonJoint& GetJoint(u8 parIndex);
    const SkelettonJoint& GetJoint(u8 parIndex) const;

private:
    u8 FBonesNumber = 0;
    SkelettonJoint* FJoints = nullptr;
};
} // namespace Rendering
} // namespace ECSEngine
