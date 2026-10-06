#include "stdafx.h"

#include "Contact.h"

#include "CapsuleIntersection.h"
#include "CapsuleSphereIntersection.h"
#include "Common/Sorting.h"
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

    bool bAnyTouching = false;
    i32 CurrentContactPoints = 0;
    float maxPenetration = -std::numeric_limits<float>::max();
    vec3 maxPenetrationNormal;
    forrange(i, 0, firstBody->FCollisionShapes.size())
    {
        forrange(j, 0, secondBody->FCollisionShapes.size())
        {
            CurrentContactPoints = FManifold.FContactPoints.size();
            CollisionShape& ShapeA = firstBody->FCollisionShapes[i];
            CollisionShape& ShapeB = secondBody->FCollisionShapes[j];
            bool bTouching = false;
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
                    bTouching = OBBCapsuleIntersection(this, secondBody->GetTransform(), ShapeB.GetCenter(), ShapeB.GetRadius(), ShapeB.GetHalfLength(), firstBody->GetTransform(),
                          ShapeA.GetLocalAABB(), false);
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
                    bTouching = SphereIntersection(this, firstBody->GetTransform(), vec4::MakeHomogeneousPositionVec4(ShapeA.GetCenter()), ShapeA.GetRadius(),
                          secondBody->GetTransform(), vec4::MakeHomogeneousPositionVec4(ShapeB.GetCenter()), ShapeB.GetRadius());
                    break;
                }
                case ECollisionShape::CAPSULE:
                {
                    bTouching = CapsuleSphereIntersection(this, secondBody->GetTransform(), ShapeB.GetCenter(), ShapeB.GetRadius(), ShapeB.GetHalfLength(),
                          firstBody->GetTransform(), ShapeA.GetCenter(), ShapeA.GetRadius(), true);
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
                    bTouching = CapsuleSphereIntersection(this, firstBody->GetTransform(), ShapeA.GetCenter(), ShapeA.GetRadius(), ShapeA.GetHalfLength(),
                          secondBody->GetTransform(), ShapeB.GetCenter(), ShapeB.GetRadius(), false);
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

            // Handle the pair result;
            if (bTouching)
            {
                bAnyTouching = true;
                forrange(k, CurrentContactPoints, FManifold.FContactPoints.size())
                {
                    ContactPoint& cp = FManifold.FContactPoints[k];
                    cp.FP.ShapeAIndex = i;
                    cp.FP.ShapeBIndex = j;

                    if (cp.FPenetration > maxPenetration)
                    {
                        maxPenetration = cp.FPenetration;
                        maxPenetrationNormal = FContactNormal;
                    }
                }
            }
        }
    }

    // Deepest first so Island's 8-point cap keeps the most penetrating contacts.
    if (!FManifold.FContactPoints.empty())
    {
        auto SortByPenetrationDesc = [](const ContactPoint& A, const ContactPoint& B) -> bool { return A.FPenetration > B.FPenetration; };
        InPlaceSorting<std::vector<ContactPoint>, ContactPoint>(
              FManifold.FContactPoints, 0, FManifold.FContactPoints.size() - 1, SortByPenetrationDesc);

        constexpr std::size_t kMaxContactPoints = 8;
        if (FManifold.FContactPoints.size() > kMaxContactPoints)
            FManifold.FContactPoints.resize(kMaxContactPoints);
    }

    if (bAnyTouching)
        FContactNormal = maxPenetrationNormal;

    const bool bWasTouching = FFlags.GetValue(EContactFlag::CT_TOUCHING);
    FFlags.SetBit(EContactFlag::CT_WAS_TOUCHING, bWasTouching);
    FFlags.SetBit(EContactFlag::CT_TOUCHING, bAnyTouching);

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
