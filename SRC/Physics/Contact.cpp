#include "stdafx.h"

#include "Contact.h"

#include "OBBIntersection.h"
#include "PhysicsEngine.h"

namespace ECSEngine
{
namespace Physics
{
IMPLEMENT_POOL_ALLOCATED(Contact);

void Contact::Evaluate()
{
    RigidBody* firstBody = PhysicsEngine::Instance().GetRigidBody(FFirstBody);
    RigidBody* secondBody = PhysicsEngine::Instance().GetRigidBody(FSecondBody);

    const bool bTouching = OBBIntersection(
          firstBody->GetTransform(), firstBody->FCollisionShape.GetLocalAABB(), secondBody->GetTransform(), secondBody->FCollisionShape.GetLocalAABB());

    FFlags.SetBit(EContactFlag::TOUCHING, bTouching);
}

} // namespace Physics
} // namespace ECSEngine
