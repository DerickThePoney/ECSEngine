#pragma once
#include "AABBTree.h"

namespace ECSEngine
{
namespace Physics
{
class BroadPhase
{
public:
    void UpdatePairs();

    void AddNewBody(RigidBody* body);
    void RemoveBody(RigidBody* body);

    void MoveBody(RigidBody* body, vec3 displacement);

#ifdef PERFORM_SECURITY_CHECKS
    void DebugBroadPhase();
#endif

private:
    AABBTree FTree;
};
} // namespace Physics
} // namespace ECSEngine