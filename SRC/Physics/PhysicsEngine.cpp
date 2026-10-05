#include "stdafx.h"

#include "PhysicsEngine.h"

#include "PhysicsBodyConfig.h"
#include "PhysicsBodyHandle.h"

#ifdef PERFORM_SECURITY_CHECKS
#include "Common/CameraHelpers.h"
#include "Common/CameraManager.h"
#include "Common/ColorUtils.h"
#include "Common/InputManager.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#endif

namespace ECSEngine
{
namespace Physics
{
PhysicsEngine::PhysicsEngine()
    : Singleton()
{
}

PhysicsEngine::~PhysicsEngine()
{
}

void PhysicsEngine::Initialize(const PhysicsEngineConfiguration& PhysicsConfig)
{
    SetConfig(PhysicsConfig);
}

void PhysicsEngine::Cleanup()
{
    FContactManager.Shutdown();
    FRigidbodies.clear();
}

void PhysicsEngine::UpdatePhysics(float parDeltaTime)
{
    FContactManager.FindNewContacts(FBroadPhase);

    FContactManager.CollideContacts(this, FBroadPhase);

    FIslandManager.SolveIslands(this, parDeltaTime);

    // Update inertia tensor
    foreachitem(bodyHandle, FPhysicsRigidbodies)
    {
        if (!bodyHandle.IsValid())
        {
            continue;
        }

        RigidBody* body = FRigidbodies[bodyHandle.FId].get();
        if (body == nullptr)
        {
            continue;
        }

        UpdateInertiaTransform(body);
    }

    // Update moved bodies
    foreachitem(movedBody, FMovedBodies)
    {
        RigidBody* body = GetRigidBody(movedBody.Handle);
        if (body == nullptr)
        {
            continue;
        }
        FBroadPhase.MoveBody(body, movedBody.Displacement);
    }
    FMovedBodies.clear();
}

const PhysicsBodyHandle PhysicsEngine::CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig)
{
    PhysicsBodyHandle newHandle;

    RigidBody* newBody = new RigidBody;
    InitializeBody(newBody, Transform, BodyConfig);

    newHandle.FId = FHandleGenerator.GetNextId();
    newBody->FHandle = newHandle;

    if (FRigidbodies.size() > newHandle.FId)
    {
        AlwaysCheckedAssert(FRigidbodies[newHandle.FId] == nullptr);
        FRigidbodies[newHandle.FId].reset(newBody);
    }
    else
    {
        AssertRelease(FRigidbodies.size() == newHandle.FId);
        FRigidbodies.emplace_back(newBody);
    }

    FBroadPhase.AddNewBody(newBody);

    switch (newBody->FMoveabilityType)
    {
    case EPhysicsMoveability::STATIC:
        FStaticRigidbodies.insert(newHandle);
        break;
    case EPhysicsMoveability::KINEMATIC:
        FKinematicRigidbodies.insert(newHandle);
        break;
    case EPhysicsMoveability::PHYICS_ENABLED:
        FPhysicsRigidbodies.insert(newHandle);
        break;
    default:
        AssertNotReached();
        break;
    }
    return newHandle;
}

bool PhysicsEngine::DestroyPhysicsBody(const PhysicsBodyHandle& Handle)
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId >= FRigidbodies.size())
    {
        return false;
    }

    FContactManager.RemoveBody(Handle);
    FBroadPhase.RemoveBody(FRigidbodies[Handle.FId].get());
    FHandleGenerator.ReleaseId(Handle.FId);
    FRigidbodies[Handle.FId].reset(nullptr);

    return true;
}

#pragma region BodyGettersAndSetters
bool PhysicsEngine::SetBodyPosition(const PhysicsBodyHandle& Handle, const vec3& Position)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->FPosition = Position;
    body->UpdateCoMWorld();
    return true;
}

bool PhysicsEngine::SetBodyVelocity(const PhysicsBodyHandle& Handle, const vec3& Velocity)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->FVelocity = Velocity;
    return true;
}

bool PhysicsEngine::SetBodyOrientation(const PhysicsBodyHandle& Handle, const quat& Orientation)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->FOrientation = Orientation;
    body->UpdateCoMWorld();
    UpdateInertiaTransform(body);
    return true;
}

bool PhysicsEngine::SetBodyRotationVelocity(const PhysicsBodyHandle& Handle, const vec3& RotationVelocity)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->FRotationVelocity = RotationVelocity;
    return true;
}

bool PhysicsEngine::GetBodyPosition(const PhysicsBodyHandle& Handle, vec3& Position) const
{
    const RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    Position = body->FPosition;
    return true;
}

bool PhysicsEngine::GetBodyVelocity(const PhysicsBodyHandle& Handle, vec3& Velocity) const
{
    const RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    Velocity = body->FVelocity;
    return true;
}

bool PhysicsEngine::GetBodyOrientation(const PhysicsBodyHandle& Handle, quat& Orientation) const
{
    const RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    Orientation = body->FOrientation;
    return true;
}

bool PhysicsEngine::GetBodyRotationVelocity(const PhysicsBodyHandle& Handle, vec3& RotationVelocity) const
{
    const RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    RotationVelocity = body->FRotationVelocity;
    return true;
}

#pragma endregion BodyGettersAndSetters

#pragma region ForcesAndTorques
bool PhysicsEngine::AddForceToBody(const PhysicsBodyHandle& Handle, const vec3& Force, bool bTreatAsAcceleration)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->AddForce(Force, bTreatAsAcceleration);

    return true;
}

bool PhysicsEngine::AddForceAtPointToBody(const PhysicsBodyHandle& Handle, const vec3& Force, const vec3& Point, bool bTreatPointAsLocalCoord, bool bTreatAsAcceleration)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    vec3 PointToUse = Point;
    if (bTreatPointAsLocalCoord)
    {
        PointToUse = (Translation(body->FPosition) * (mat4)body->FOrientation * vec4::MakeHomogeneousPositionVec4(Point)).xyz();
    }

    PointToUse -= body->FCenterOfMassWorld;

    body->AddForce(Force, bTreatAsAcceleration);

    vec3 TorqueToAdd = Cross(PointToUse, Force);

    body->AddTorque(TorqueToAdd);

    return true;
}

bool PhysicsEngine::AddTorqueToBody(const PhysicsBodyHandle& Handle, const vec3& Torque)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->AddTorque(Torque);
    return true;
}
#pragma endregion ForcesAndTorques

#pragma region Impulses
bool PhysicsEngine::AddImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, bool bTreatAsVelocityChange)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->AddImpulse(Impulse, bTreatAsVelocityChange);
    return true;
}

bool PhysicsEngine::AddImpulseAtPointToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, const vec3& Point, bool bTreatPointAsLocalCoord, bool bTreatAsVelocityChange)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    vec3 PointToUse = Point;
    if (bTreatPointAsLocalCoord)
    {
        PointToUse = (Translation(body->FPosition) * (mat4)body->FOrientation * vec4::MakeHomogeneousPositionVec4(Point)).xyz();
    }

    PointToUse -= body->FCenterOfMassWorld;
    body->AddImpulse(Impulse, bTreatAsVelocityChange);

    vec3 RotationImpulseToAdd = Cross(PointToUse, Impulse);
    body->AddRotationImpulse(RotationImpulseToAdd, bTreatAsVelocityChange);
    return true;
}

bool PhysicsEngine::AddRotationImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& RotationImpulse, bool bTreatAsRotationVelocityChange)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->AddRotationImpulse(RotationImpulse, bTreatAsRotationVelocityChange);
    return true;
}

#pragma endregion Impulses

void PhysicsEngine::InitializeBody(RigidBody* Body, const mat4& Transform, const PhysicsBodyConfig& BodyConfig)
{
    AssertRelease(Body != nullptr);

    // Moveability
    Body->FMoveabilityType = BodyConfig.FMoveability;

    // Init position and orientation
    Body->FPosition = Transform.Column(3).xyz();
    Body->FOrientation = quat::FromMat4(Transform);

    if (Body->FMoveabilityType == EPhysicsMoveability::PHYICS_ENABLED)
    {
        // Init Mass and Inertia
        if (BodyConfig.FAutoComputeMass)
        {
            Body->FMass = BodyConfig.FShape.ComputeMass(BodyConfig.FDensity);
        }
        else
        {
            Body->FMass = BodyConfig.FMass;
        }

        Body->FInvMass = (Body->FMass != 0.f) ? 1.f / Body->FMass : 1.f;
        Body->FInertiaTensor = BodyConfig.FShape.ComputeInertiaTensor(Body->FMass);
        Body->FInverseInertiaTensor = Invert(Body->FInertiaTensor);
    }
    else
    {
        Body->FMass = 0.f;
        Body->FInvMass = 0.f;
        Body->FInertiaTensor = mat3();
        Body->FInverseInertiaTensor = mat3();
    }

    Body->FCenterOfMassLocal = BodyConfig.ComputeCoMLocal();
    Body->FCenterOfMassWorld = (Transform * vec4::MakeHomogeneousPositionVec4(Body->FCenterOfMassLocal)).xyz();

    Body->FCollisionShape = BodyConfig.FShape;

    // Init damping coefficients
    Body->FLinearDamping = BodyConfig.FLinearDamping;
    Body->FAngularDamping = BodyConfig.FAngularDamping;
    if (!BodyConfig.FApplyGravity)
        Body->FGravityScale = 0.f;

    UpdateInertiaTransform(Body);

    Body->SetAwake(true);
}

void PhysicsEngine::UpdateInertiaTransform(RigidBody* Body)
{
    AssertRelease(Body != nullptr);

    // Normalize orientation
    Body->FOrientation = Normalize(Body->FOrientation);

    // recompute the inverse inertia tensor in world coordinate using Mt' = Mb * Mt * Mb^-1
    mat3 worldRotation = GetRotation((mat4)Body->FOrientation);
    Body->FInverseInertiaTensorWorld = worldRotation * Body->FInverseInertiaTensor * Transpose(worldRotation);
}

RigidBody* PhysicsEngine::GetRigidBody(const PhysicsBodyHandle& Handle)
{
    if (!Handle.IsValid())
    {
        return nullptr;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return nullptr;
    }

    return FRigidbodies[Handle.FId].get();
}

const RigidBody* PhysicsEngine::GetRigidBody(const PhysicsBodyHandle& Handle) const
{
    if (!Handle.IsValid())
    {
        return nullptr;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return nullptr;
    }

    return FRigidbodies[Handle.FId].get();
}

void PhysicsEngine::AddMovedBody(RigidBody* body, vec3 displacement)
{
    AlwaysCheckedAssert(body != nullptr);
    if (body == nullptr)
    {
        return;
    }
    FMovedBodies.push_back({ body->FHandle, displacement });
}

#ifdef PERFORM_SECURITY_CHECKS
void PhysicsEngine::DrawDebug()
{
    DrawDebugInternal();
    FBroadPhase.DebugBroadPhase();
    FContactManager.DebugDrawContacts(FBroadPhase);
}

void PhysicsEngine::DrawDebugInternal()
{
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bDrawCollisionShapes, false, "Draw CollisionShapes", "Physics/Collisions");
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bShowSleepingShapes, false, "Show SleepingShapes", "Physics/Collisions");

    if (bDrawCollisionShapes)
    {
        Rendering::DrawCommandBuffer* buffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::DEBUG_PASS);
        u32 camId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
        Camera* camera = CameraManager::Instance().GetCamera(camId);
        buffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio()));
        Rendering::MaterialInstanceHandle handle = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");

        static const u32 DynamicShapeColor = ColorUtils::ConvertToU32(vec4(0.f, 1.f, 0.f, 1.f));
        static const u32 StaticShapeColor = ColorUtils::ConvertToU32(vec4(1.f, 0.f, 0.f, 1.f));
        static const u32 SleepingShapeColor = ColorUtils::ConvertToU32(vec4(1.f, 0.f, 1.f, 1.f));

        foreachitemconst(bodyU, FRigidbodies)
        {
            const RigidBody* body = bodyU.get();
            if (body == nullptr)
            {
                continue;
            }

            u32 ShapeColor = DynamicShapeColor;
            if (body->FMoveabilityType == EPhysicsMoveability::STATIC)
            {
                ShapeColor = StaticShapeColor;
            }
            else if (bShowSleepingShapes && !body->FFlags.GetValue(ERigidBodyFlag::RB_AWAKE))
            {
                ShapeColor = SleepingShapeColor;
            }

            switch (body->FCollisionShape.GetShapeType())
            {
            case ECollisionShape::BOX:
            {
                AABB3f LocalAABB = body->FCollisionShape.GetLocalAABB();
                buffer->DrawOOB(handle, LocalAABB.Min(), LocalAABB.Max(), body->GetTransform(), ShapeColor);
                break;
            }
            case ECollisionShape::SPHERE:
            {
                buffer->DrawDebugSphere(
                      handle, (body->GetTransform() * vec4::MakeHomogeneousPositionVec4(body->FCollisionShape.GetCenter())).xyz(), body->FCollisionShape.GetRadius(), ShapeColor);
                break;
            }
            case ECollisionShape::CAPSULE:
            {
                const mat4 Tr = body->GetTransform();
                const vec4 CapsuleCenter = (Tr * vec4::MakeHomogeneousPositionVec4(body->FCollisionShape.GetCenter()));
                const vec4 CapsuleAxis = Tr.Column(1);
                const float HalfLength = body->FCollisionShape.GetHalfLength();
                const float Radius = body->FCollisionShape.GetRadius();

                const vec4 Ac = CapsuleCenter - CapsuleAxis * HalfLength;
                const vec4 Bc = CapsuleCenter + CapsuleAxis * HalfLength;

                buffer->DrawDebugSphere(handle, Ac.xyz(), body->FCollisionShape.GetRadius(), ShapeColor);
                buffer->DrawDebugSphere(handle, Bc.xyz(), body->FCollisionShape.GetRadius(), ShapeColor);

                break;
            }
            default:
                AssertNotReached();
            }
        }

        buffer->Submit();
        Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(buffer);
    }
}
#endif

} // namespace Physics
} // namespace ECSEngine
