#pragma once

namespace ECSEngine
{
namespace Physics
{
class RigidBody;
class Island
{
public:
    void Init(u32 BodyCount, u32 ContactCount);
    void Reset();

private:
    std::vector<RigidBody*> Bodies;
};
} // namespace Physics
} // namespace ECSEngine