#include "stdafx.h"

#include "BroadPhase.h"

namespace ECSEngine
{
namespace Physics
{

void BroadPhase::UpdatePairs()
{
}

void BroadPhase::AddNewBody(RigidBody* body)
{
    FTree.InsertBody(body);
}

void BroadPhase::RemoveBody(RigidBody* body)
{
    FTree.RemoveBody(body);
}

void BroadPhase::MoveBody(RigidBody* body, vec3 displacement)
{
    FTree.MoveBody(body, displacement);
}

#ifdef PERFORM_SECURITY_CHECKS
void BroadPhase::DebugBroadPhase()
{
    FTree.DebugTree();
}
#endif

} // namespace Physics
} // namespace ECSEngine