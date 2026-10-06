#include "stdafx.h"

#include "Contact.h"

#include "CapsuleIntersection.h"
#include "CapsuleSphereIntersection.h"
#include "OBBCapsuleIntersection.h"
#include "OBBIntersection.h"
#include "OBBSphereIntersection.h"
#include "PhysicsEngine.h"
#include "SphereIntersection.h"

namespace ECSEngine
{
namespace Physics
{
IMPLEMENT_POOL_ALLOCATED(ContactPoint);
IMPLEMENT_POOL_ALLOCATED(Contact);

void Contact::Evaluate()
{
    RigidBody* firstBody = PhysicsEngine::Instance().GetRigidBody(FFirstBody);
    RigidBody* secondBody = PhysicsEngine::Instance().GetRigidBody(FSecondBody);

    FManifold.clear();

    bool bTouching = false;
    CollisionShape& ShapeA = firstBody->FCollisionShapes[0];
    CollisionShape& ShapeB = secondBody->FCollisionShapes[0];
    switch (ShapeA.GetShapeType())
    {
    case ECollisionShape::BOX:
    {
        switch (ShapeB.GetShapeType())
        {
        case ECollisionShape::BOX:
        {
            bTouching = OBBIntersection(this, firstBody->GetTransform(), ShapeA.GetLocalAABB(), secondBody->GetTransform(), ShapeB.GetLocalAABB());
            break;
        }
        case ECollisionShape::SPHERE:
        {
            bTouching = OBBSphereIntersection(this, firstBody->GetTransform(), ShapeA.GetLocalAABB(), secondBody->GetTransform(),
                  vec4::MakeHomogeneousPositionVec4(ShapeB.GetCenter()), ShapeB.GetRadius(), false);
            break;
        }
        case ECollisionShape::CAPSULE:
        {
            bTouching = OBBCapsuleIntersection(
                  this, secondBody->GetTransform(), ShapeB.GetCenter(), ShapeB.GetRadius(), ShapeB.GetHalfLength(), firstBody->GetTransform(), ShapeA.GetLocalAABB(), false);
            break;
        }
        default:
            AssertNotReachedMsg("Collision method for BOX to ??? is not implemented !");
        }
        break;
    }
    case ECollisionShape::SPHERE:
        switch (ShapeB.GetShapeType())
        {
        case ECollisionShape::BOX:
        {
            bTouching = OBBSphereIntersection(this, secondBody->GetTransform(), ShapeB.GetLocalAABB(), firstBody->GetTransform(),
                  vec4::MakeHomogeneousPositionVec4(ShapeA.GetCenter()), ShapeA.GetRadius(), true);
            break;
        }
        case ECollisionShape::SPHERE:
        {
            bTouching = SphereIntersection(this, firstBody->GetTransform(), vec4::MakeHomogeneousPositionVec4(ShapeA.GetCenter()), ShapeA.GetRadius(), secondBody->GetTransform(),
                  vec4::MakeHomogeneousPositionVec4(ShapeB.GetCenter()), ShapeB.GetRadius());
            break;
        }
        case ECollisionShape::CAPSULE:
        {
            bTouching = CapsuleSphereIntersection(this, secondBody->GetTransform(), ShapeB.GetCenter(), ShapeB.GetRadius(), ShapeB.GetHalfLength(), firstBody->GetTransform(),
                  ShapeA.GetCenter(), ShapeA.GetRadius(), true);
            break;
        }
        default:
            AssertNotReachedMsg("Collision method for SPHERE to ??? is not implemented !");
        }
        break;
    case ECollisionShape::CAPSULE:
    {
        switch (ShapeB.GetShapeType())
        {
        case ECollisionShape::BOX:
        {
            bTouching = OBBCapsuleIntersection(
                  this, firstBody->GetTransform(), ShapeA.GetCenter(), ShapeA.GetRadius(), ShapeA.GetHalfLength(), secondBody->GetTransform(), ShapeB.GetLocalAABB(), true);
            break;
        }
        case ECollisionShape::SPHERE:
        {
            bTouching = CapsuleSphereIntersection(this, firstBody->GetTransform(), ShapeA.GetCenter(), ShapeA.GetRadius(), ShapeA.GetHalfLength(), secondBody->GetTransform(),
                  ShapeB.GetCenter(), ShapeB.GetRadius(), false);
            break;
        }
        case ECollisionShape::CAPSULE:
        {
            bTouching = CapsuleIntersection(this, firstBody->GetTransform(), ShapeA.GetCenter(), ShapeA.GetRadius(), ShapeA.GetHalfLength(), secondBody->GetTransform(),
                  ShapeB.GetCenter(), ShapeB.GetRadius(), ShapeB.GetHalfLength());
            break;
        }
        default:
            AssertNotReachedMsg("Collision method for SPHERE to ??? is not implemented !");
        }
        break;
    }
    default:
        AssertNotReachedMsg("Collision method is not implemented !");
    }

    const bool bWasTouching = FFlags.GetValue(EContactFlag::CT_TOUCHING);
    FFlags.SetBit(EContactFlag::CT_WAS_TOUCHING, bWasTouching);
    FFlags.SetBit(EContactFlag::CT_TOUCHING, bTouching);

    ComputeBasis();
}

// http://box2d.org/2014/02/computing-a-basis/
void Contact::ComputeBasis()
{
    // Suppose vector a has all equal components and is a unit vector: a = (s, s, s)
    // Then 3*s*s = 1, s = sqrt(1/3) = 0.57735027. This means that at least one component of a
    // unit vector must be greater or equal to 0.57735027. Can use SIMD select operation.

    if (fabsf(FContactNormal.x) >= 0.57735027f)
        FContactTangents[0] = vec3(FContactNormal.y, -FContactNormal.x, 0.f);
    else
        FContactTangents[0] = vec3(0.f, FContactNormal.z, -FContactNormal.y);

    FContactTangents[0] = Normalize(FContactTangents[0]);
    FContactTangents[1] = Cross(FContactNormal, FContactTangents[0]);
}

} // namespace Physics
} // namespace ECSEngine
