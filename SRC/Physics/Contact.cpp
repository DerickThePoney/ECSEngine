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

    bool bTouching = false;
    switch (firstBody->FCollisionShape.GetShapeType())
    {
    case ECollisionShape::BOX:
    {
        switch (secondBody->FCollisionShape.GetShapeType())
        {
        case ECollisionShape::BOX:
        {
            bTouching = OBBIntersection(
                  this, firstBody->GetTransform(), firstBody->FCollisionShape.GetLocalAABB(), secondBody->GetTransform(), secondBody->FCollisionShape.GetLocalAABB());
            break;
        }
        case ECollisionShape::SPHERE:
        {
        }
        default:
            AssertNotReachedMsg("Collision method is not implemented !");
        }
        break;
    }
    case ECollisionShape::SPHERE:
    default:
        AssertNotReachedMsg("Collision method is not implemented !");
    }
    FFlags.SetBit(EContactFlag::TOUCHING, bTouching);
}

} // namespace Physics
} // namespace ECSEngine
