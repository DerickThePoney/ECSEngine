#include "stdafx.h"

#include "PhysicsJointsManager.h"

#include "Common/Logger.h"
#include "PhysicsEngine.h"
#include "RigidBody.h"

#ifdef PERFORM_SECURITY_CHECKS
#include "Common/CameraHelpers.h"
#include "Common/CameraManager.h"
#include "Common/ColorUtils.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "JointBatchHelpers.h"
#include "Math/MatrixUtils.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#endif

namespace ECSEngine
{
namespace Physics
{
void PhysicsJointsManager::RemoveBody(RigidBody* Body)
{
    foreachitemconst(jointHandle, Body->FJoints)
    {
        DestroyJoint(jointHandle);
    }

    Body->FJoints.clear();
}

u16 GetNextJointId(IdGenerator& Generator)
{
    u32 Id = Generator.GetNextId();
    if ((Id & 0xFFFF0000) > 0)
    {
        LOG_ERROR("Already filled the joints buffer for this buffer");
        AssertNotReached();
        return (u16)~0;
    }
    return (u16)Id;
}

template<typename Container, typename JointConfig>
PhysicsJointHandle
CreateJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, Container& JointContainer, const JointConfig& Config, EJointType JointType, IdGenerator& Generator)
{
    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(BodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(BodyB);

    if (bodyA == nullptr || bodyB == nullptr)
    {
        LOG_ERROR("Trying to attach stale bodies...");
        return PhysicsJointHandle{ };
    }

    u16 Id = GetNextJointId(Generator);

    if (Id == (u16)~0)
    {
        return PhysicsJointHandle{ };
    }

    if (Id >= JointContainer.size())
    {
        JointContainer.resize(Id + 1);
        JointContainer[Id].Handle.FIndex = Id;
        JointContainer[Id].Handle.FType = (u8)JointType;
        JointContainer[Id].Handle.FGeneration = 0;
    }

    // Fill up joint data
    JointContainer[Id].Joint.Initialize(BodyA, BodyB, Config);
    bodyA->FJoints.push_back(JointContainer[Id].Handle);
    bodyB->FJoints.push_back(JointContainer[Id].Handle);

    bodyA->SetAwake(true);
    bodyB->SetAwake(true);

    return JointContainer[Id].Handle;
}

PhysicsJointHandle PhysicsJointsManager::CreateDistanceJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config)
{
    if (BodyA == BodyB)
    {
        LOG_ERROR("Nice try young padawan. But attaching the same body to the same joint is not supported !");
        return PhysicsJointHandle{ };
    }

    return CreateJoint(BodyA, BodyB, FDistanceJoints, Config, EJointType::DISTANCE, FDistanceJointsIdGenerator);
}

PhysicsJointHandle PhysicsJointsManager::CreateBallJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const BallJointConfig& Config)
{
    if (BodyA == BodyB)
    {
        LOG_ERROR("Nice try young padawan. But attaching the same body to the same joint is not supported !");
        return PhysicsJointHandle{ };
    }

    return CreateJoint(BodyA, BodyB, FBallJoints, Config, EJointType::BALL, FBallJointsIdGenerator);
}

PhysicsJointHandle PhysicsJointsManager::CreateHingeJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const HingeJointConfig& Config)
{
    if (BodyA == BodyB)
    {
        LOG_ERROR("Nice try young padawan. But attaching the same body to the same joint is not supported !");
        return PhysicsJointHandle{ };
    }

    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(BodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(BodyB);

    if (bodyA == nullptr || bodyB == nullptr)
    {
        LOG_ERROR("Trying to attach stale bodies...");
        return PhysicsJointHandle{ };
    }

    return CreateJoint(BodyA, BodyB, FHingeJoints, Config, EJointType::HINGE, FHingeJointsIdGenerator);
}

template<typename Container>
void DestroyJointTemplate(const PhysicsJointHandle& Handle, Container& JointContainer, IdGenerator& Generator, EJointType JointType)
{
    if (Handle.GetTypeAsEnum() != JointType)
    {
        LOG_ERROR(std::format("Trying to delete a joint (type {}) with a handle for type {}", (u8)JointType, Handle.FType).c_str());
        return;
    }

    if (Handle.FIndex >= JointContainer.size())
    {
        LOG_ERROR(std::format("Joint handle index is out of bound: {} - Count {}", Handle.FIndex, JointContainer.size()).c_str());
        return;
    }

    if (Handle.FGeneration != JointContainer[Handle.FIndex].Handle.FGeneration)
    {
        LOG_ERROR(
              std::format("Trying to destroy an outdated joint: Handle Generation {} - Current Generation {}", Handle.FGeneration, JointContainer[Handle.FIndex].Handle.FGeneration)
                    .c_str());
        return;
    }

    // Detach the rigidbodies
    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(JointContainer[Handle.FIndex].Joint.FBodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(JointContainer[Handle.FIndex].Joint.FBodyB);
    if (bodyA != nullptr)
    {
        for (auto it = bodyA->FJoints.begin(); it != bodyA->FJoints.end(); ++it)
        {
            if (*it == Handle)
            {
                bodyA->FJoints.erase(it);
                break;
            }
        }

        bodyA->SetAwake(true);
    }

    if (bodyB != nullptr)
    {
        for (auto it = bodyB->FJoints.begin(); it != bodyB->FJoints.end(); ++it)
        {
            if (*it == Handle)
            {
                bodyB->FJoints.erase(it);
                break;
            }
        }
        bodyB->SetAwake(true);
    }

    // Just Increase the generation.
    JointContainer[Handle.FIndex].Handle.FGeneration++;
    JointContainer[Handle.FIndex].Joint.Destroy();

    Generator.ReleaseId(Handle.FIndex);
}

void PhysicsJointsManager::DestroyJoint(const PhysicsJointHandle& Handle)
{
    if (!Handle.IsValid())
    {
        return;
    }

    switch (Handle.GetTypeAsEnum())
    {
    case EJointType::DISTANCE:
    {
        DestroyJointTemplate(Handle, FDistanceJoints, FDistanceJointsIdGenerator, EJointType::DISTANCE);
        break;
    }
    case EJointType::BALL:
    {
        DestroyJointTemplate(Handle, FBallJoints, FBallJointsIdGenerator, EJointType::BALL);
        break;
    }
    case EJointType::HINGE:
    {
        DestroyJointTemplate(Handle, FHingeJoints, FHingeJointsIdGenerator, EJointType::HINGE);
        break;
    }
    default:
        AssertNotReached();
    }
}

JointBase* PhysicsJointsManager::GetJoint(const PhysicsJointHandle& Handle)
{
    if (!Handle.IsValid())
        return nullptr;

    switch (Handle.GetTypeAsEnum())
    {
    case EJointType::DISTANCE:
    {
        if (Handle.FIndex >= FDistanceJoints.size())
            return nullptr;
        if (FDistanceJoints[Handle.FIndex].Handle != Handle)
            return nullptr;
        return &FDistanceJoints[Handle.FIndex].Joint;
        break;
    }
    case EJointType::BALL:
    {
        if (Handle.FIndex >= FBallJoints.size())
            return nullptr;
        if (FBallJoints[Handle.FIndex].Handle != Handle)
            return nullptr;
        return &FBallJoints[Handle.FIndex].Joint;
        break;
    }
    case EJointType::HINGE:
    {
        if (Handle.FIndex >= FHingeJoints.size())
            return nullptr;
        if (FHingeJoints[Handle.FIndex].Handle != Handle)
            return nullptr;
        return &FHingeJoints[Handle.FIndex].Joint;
        break;
    }
    default:
        AssertNotReached();
    }

    return nullptr;
}

void PhysicsJointsManager::PreUpdate()
{
    foreachitem(joint, FDistanceJoints)
    {
        joint.Joint.bIsland = false;
    }

    foreachitem(joint, FBallJoints)
    {
        joint.Joint.bIsland = false;
    }

    foreachitem(joint, FHingeJoints)
    {
        joint.Joint.bIsland = false;
    }
}

void PhysicsJointsManager::SetHingeMotor(const PhysicsJointHandle& JointHandle, bool bEnable, float MotorSpeed, float MaxTorque)
{
    if (!JointHandle.IsValid())
        return;

    if (JointHandle.GetTypeAsEnum() != EJointType::HINGE)
    {
        LOG_ERROR(std::format("Try to set hinge motor up for joint of type {}", JointHandle.FType));
        return;
    }

    HingeJoint* joint = dynamic_cast<HingeJoint*>(GetJoint(JointHandle));
    if (joint == nullptr)
    {
        return;
    }

    joint->FEnableMotor = bEnable;
    joint->FMotorSpeed = MotorSpeed;
    joint->FMaxMotorTorque = MaxTorque;
}

void PhysicsJointsManager::SetHingeTargetAngle(const PhysicsJointHandle& JointHandle, bool bEnable, float TargetAngle, float Stiffness, float MaxTorque)
{
    if (!JointHandle.IsValid())
        return;

    if (JointHandle.GetTypeAsEnum() != EJointType::HINGE)
    {
        LOG_ERROR(std::format("Try to set hinge motor up for joint of type {}", JointHandle.FType));
        return;
    }

    HingeJoint* joint = dynamic_cast<HingeJoint*>(GetJoint(JointHandle));
    if (joint == nullptr)
    {
        return;
    }

    joint->FEnableMotor = bEnable;
    joint->FUseTargetAngle = bEnable;
    joint->FTargetAngle = TargetAngle;
    joint->FMotorStiffness = Stiffness;
    joint->FMaxMotorTorque = MaxTorque;
}

void PhysicsJointsManager::SetBallMotor(const PhysicsJointHandle& JointHandle, bool bEnable, const vec3& VelocityInA, float MaxTorque)
{
    if (!JointHandle.IsValid())
        return;

    if (JointHandle.GetTypeAsEnum() != EJointType::BALL)
    {
        LOG_ERROR(std::format("Try to set ball motor up for joint of type {}", JointHandle.FType));
        return;
    }

    BallJoint* joint = dynamic_cast<BallJoint*>(GetJoint(JointHandle));
    if (joint == nullptr)
    {
        return;
    }

    joint->FEnableMotor = bEnable;
    joint->FMotorVelocityLocal = VelocityInA;
    joint->FMaxMotorTorque = MaxTorque;
}

void PhysicsJointsManager::SetBallTargetEulerAngles(const PhysicsJointHandle& JointHandle, bool bEnable, const vec3& EulerYawRollPitch, float Stiffness, float MaxTorque)
{
    if (!JointHandle.IsValid())
        return;

    if (JointHandle.GetTypeAsEnum() != EJointType::BALL)
    {
        LOG_ERROR(std::format("Try to set ball motor up for joint of type {}", JointHandle.FType));
        return;
    }

    BallJoint* joint = dynamic_cast<BallJoint*>(GetJoint(JointHandle));
    if (joint == nullptr)
    {
        return;
    }

    joint->FEnableMotor = bEnable;
    joint->FTargetEulerAngles = EulerYawRollPitch;
    joint->FMotorStiffness = Stiffness;
    joint->FMaxMotorTorque = MaxTorque;
}

#ifdef PERFORM_SECURITY_CHECKS
namespace
{
struct JointDebugDrawFlags
{
    bool bDrawLimits = true;
    bool bDrawMotors = true;
    bool bDrawImpulses = false;
    float ImpulseArrowScale = 0.1f;
};

vec3 JointWorldAnchor(const RigidBody* Body, const vec3& AnchorLocal)
{
    return (Body->GetTransform() * vec4::MakeHomogeneousPositionVec4(AnchorLocal)).xyz();
}

vec3 JointWorldDir(const RigidBody* Body, const vec3& LocalDir)
{
    return Normalize((Body->GetTransform() * vec4::MakeHomogeneousDirectionVec4(LocalDir)).xyz());
}

vec3 RotateAroundAxis(const vec3& Axis, float Angle, const vec3& V)
{
    return (Rotation(Angle, Axis) * vec4::MakeHomogeneousDirectionVec4(V)).xyz();
}

vec3 ProjectOnPlane(const vec3& V, const vec3& Normal)
{
    return V - Normal * Dot(V, Normal);
}

// DrawLines3DCommand stores a raw pointer and only reads it on Submit — stack arrays are invalid by then.
struct JointDebugLineScratch
{
    std::list<std::vector<vec3>> Batches;

    void DrawLine(Rendering::DrawCommandBuffer* Buffer, const Rendering::MaterialInstanceHandle& Material, const vec3& A, const vec3& B, u32 Color)
    {
        Batches.push_back({ A, B });
        const std::vector<vec3>& Verts = Batches.back();
        Buffer->DrawLines(Material, Verts.data(), (u32)Verts.size(), Color, false);
    }

    void DrawPolyline(Rendering::DrawCommandBuffer* Buffer, const Rendering::MaterialInstanceHandle& Material, std::vector<vec3>&& Verts, u32 Color, bool bClose)
    {
        Batches.push_back(std::move(Verts));
        const std::vector<vec3>& Stored = Batches.back();
        Buffer->DrawLines(Material, Stored.data(), (u32)Stored.size(), Color, bClose);
    }
};

void DrawAnchorPair(Rendering::DrawCommandBuffer* Buffer, const Rendering::MaterialInstanceHandle& Material, JointDebugLineScratch& Lines, const vec3& AnchorA, const vec3& AnchorB,
                    u32 Color)
{
    constexpr float Radius = 0.05f;
    Buffer->DrawDebugSphere(Material, AnchorA, Radius, Color);
    Buffer->DrawDebugSphere(Material, AnchorB, Radius, Color);
    Lines.DrawLine(Buffer, Material, AnchorA, AnchorB, Color);
}

void DrawImpulseArrow(Rendering::DrawCommandBuffer* Buffer, const Rendering::MaterialInstanceHandle& Material, const vec3& Origin, const vec3& Impulse, float Scale, u32 Color)
{
    const float Mag = Length(Impulse);
    if (Mag < 1e-5f)
        return;
    Buffer->DrawDebugArrow(Material, Origin, Impulse * (1.f / Mag), Mag * Scale, Color);
}

void DrawConeLimit(Rendering::DrawCommandBuffer* Buffer, const Rendering::MaterialInstanceHandle& Material, JointDebugLineScratch& Lines, const vec3& Pivot, const vec3& AxisA,
                   float MaxAngle, u32 Color)
{
    vec3 T1, T2;
    JointComputeBasis(AxisA, T1, T2);

    constexpr float RayLen = 0.35f;
    constexpr int RingCount = 16;
    std::vector<vec3> Ring;
    Ring.reserve(RingCount);
    const float CosA = cosf(MaxAngle);
    const float SinA = sinf(MaxAngle);
    for (int i = 0; i < RingCount; ++i)
    {
        const float Phi = (2.f * Pi() * (float)i) / (float)RingCount;
        const vec3 Dir = Normalize(AxisA * CosA + (T1 * cosf(Phi) + T2 * sinf(Phi)) * SinA);
        const vec3 End = Pivot + Dir * RayLen;
        Ring.push_back(End);
        if ((i % 4) == 0)
            Lines.DrawLine(Buffer, Material, Pivot, End, Color);
    }
    Lines.DrawPolyline(Buffer, Material, std::move(Ring), Color, true);
}

void DrawDistanceJointDebug(Rendering::DrawCommandBuffer* Buffer, const Rendering::MaterialInstanceHandle& Material, JointDebugLineScratch& Lines, const DistanceJoint& Joint,
                            const RigidBody* BodyA, const RigidBody* BodyB, const JointDebugDrawFlags& Flags, u32 AnchorColor, u32 ImpulseColor)
{
    const vec3 AnchorA = JointWorldAnchor(BodyA, Joint.FConfig.FCommon.FAnchorALocal);
    const vec3 AnchorB = JointWorldAnchor(BodyB, Joint.FConfig.FCommon.FAnchorBLocal);
    DrawAnchorPair(Buffer, Material, Lines, AnchorA, AnchorB, AnchorColor);

    if (Flags.bDrawImpulses)
    {
        const vec3 Mid = (AnchorA + AnchorB) * 0.5f;
        vec3 Dir = AnchorB - AnchorA;
        const float Len = Length(Dir);
        if (Len > 1e-5f)
            Dir *= 1.f / Len;
        else
            Dir = vec3(1.f, 0.f, 0.f);
        Buffer->DrawDebugArrow(Material, Mid, Dir, fabsf(Joint.FImpulse) * Flags.ImpulseArrowScale, ImpulseColor);
    }
}

void DrawBallJointDebug(Rendering::DrawCommandBuffer* Buffer, const Rendering::MaterialInstanceHandle& Material, JointDebugLineScratch& Lines, const BallJoint& Joint,
                        const RigidBody* BodyA, const RigidBody* BodyB, const JointDebugDrawFlags& Flags, u32 AnchorColor, u32 AxisColor, u32 LimitColor, u32 LimitActiveColor,
                        u32 MotorColor, u32 ImpulseColor)
{
    const vec3 AnchorA = JointWorldAnchor(BodyA, Joint.FConfig.FCommon.FAnchorALocal);
    const vec3 AnchorB = JointWorldAnchor(BodyB, Joint.FConfig.FCommon.FAnchorBLocal);
    const vec3 Pivot = (AnchorA + AnchorB) * 0.5f;
    DrawAnchorPair(Buffer, Material, Lines, AnchorA, AnchorB, AnchorColor);

    const vec3 AxisA = JointWorldDir(BodyA, Joint.FAxisALocal);
    const vec3 AxisB = JointWorldDir(BodyB, Joint.FAxisBLocal);
    Buffer->DrawDebugArrow(Material, Pivot, AxisA, 0.4f, AxisColor);
    Buffer->DrawDebugArrow(Material, Pivot, AxisB, 0.35f, AxisColor);

    if (Flags.bDrawLimits && Joint.FConfig.FEnableConeLimit)
    {
        const u32 Color = (fabsf(Joint.FConeLimitImpulse) > 1e-5f) ? LimitActiveColor : LimitColor;
        DrawConeLimit(Buffer, Material, Lines, Pivot, AxisA, Joint.FConfig.FMaxConeAngle, Color);
    }

    if (Flags.bDrawMotors && Joint.FEnableMotor)
    {
        const mat4 RA = mat4(BodyA->FOrientation);
        if (Joint.FUseTargetOrientation)
        {
            const mat4 RTarget = EulerAnglesXYZ(Joint.FTargetEulerAngles.x, Joint.FTargetEulerAngles.y, Joint.FTargetEulerAngles.z);
            const mat4 RWorld = RA * RTarget;
            constexpr float TriadLen = 0.25f;
            Buffer->DrawDebugArrow(Material, Pivot, Normalize(RWorld.Column(0).xyz()), TriadLen, MotorColor);
            Buffer->DrawDebugArrow(Material, Pivot, Normalize(RWorld.Column(1).xyz()), TriadLen, MotorColor);
            Buffer->DrawDebugArrow(Material, Pivot, Normalize(RWorld.Column(2).xyz()), TriadLen, MotorColor);
        }
        else
        {
            const vec3 VelW = (RA * vec4::MakeHomogeneousDirectionVec4(Joint.FMotorVelocityLocal)).xyz();
            const float Speed = Length(VelW);
            if (Speed > 1e-4f)
                Buffer->DrawDebugArrow(Material, Pivot, VelW * (1.f / Speed), Min(Speed * 0.1f, 0.5f), MotorColor);
        }
    }

    if (Flags.bDrawImpulses)
    {
        DrawImpulseArrow(Buffer, Material, Pivot, Joint.FPointImpulse, Flags.ImpulseArrowScale, ImpulseColor);
        const mat4 RA = mat4(BodyA->FOrientation);
        const vec3 E0 = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(vec3(1, 0, 0))).xyz());
        const vec3 E1 = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(vec3(0, 1, 0))).xyz());
        const vec3 E2 = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(vec3(0, 0, 1))).xyz());
        const vec3 MotorTau = E0 * Joint.FMotorImpulse.x + E1 * Joint.FMotorImpulse.y + E2 * Joint.FMotorImpulse.z;
        DrawImpulseArrow(Buffer, Material, Pivot, MotorTau, Flags.ImpulseArrowScale, MotorColor);
    }
}

void DrawHingeJointDebug(Rendering::DrawCommandBuffer* Buffer, const Rendering::MaterialInstanceHandle& Material, JointDebugLineScratch& Lines, const HingeJoint& Joint,
                         const RigidBody* BodyA, const RigidBody* BodyB, const JointDebugDrawFlags& Flags, u32 AnchorColor, u32 AxisColor, u32 LimitColor, u32 LimitActiveColor,
                         u32 MotorColor, u32 ImpulseColor)
{
    const vec3 AnchorA = JointWorldAnchor(BodyA, Joint.FConfig.FCommon.FAnchorALocal);
    const vec3 AnchorB = JointWorldAnchor(BodyB, Joint.FConfig.FCommon.FAnchorBLocal);
    const vec3 Pivot = (AnchorA + AnchorB) * 0.5f;
    DrawAnchorPair(Buffer, Material, Lines, AnchorA, AnchorB, AnchorColor);

    const vec3 AxisA = JointWorldDir(BodyA, Joint.FAxisALocal);
    const vec3 AxisB = JointWorldDir(BodyB, Joint.FAxisBLocal);
    Buffer->DrawDebugArrow(Material, Pivot, AxisA, 0.45f, AxisColor);
    Buffer->DrawDebugArrow(Material, Pivot, AxisB, 0.35f, AxisColor);

    const vec3 RefA = Normalize((BodyA->GetTransform() * vec4::MakeHomogeneousDirectionVec4(Joint.FRefALocal)).xyz());
    const vec3 RefB = Normalize((BodyB->GetTransform() * vec4::MakeHomogeneousDirectionVec4(Joint.FRefBLocal)).xyz());

    // Same frame as the solver: theta = 0 along RefA rotated by FReferenceAngle; current pose is RefB in the hinge plane.
    vec3 ZeroDir = Normalize(ProjectOnPlane(RotateAroundAxis(AxisA, Joint.FReferenceAngle, RefA), AxisA));
    vec3 CurrentDir = ProjectOnPlane(RefB, AxisA);
    const float CurrentLenSq = Dot(CurrentDir, CurrentDir);
    if (CurrentLenSq > 1e-8f)
        CurrentDir *= 1.f / sqrtf(CurrentLenSq);
    else
        CurrentDir = ZeroDir;

    constexpr float RayLen = 0.3f;
    Lines.DrawLine(Buffer, Material, Pivot, Pivot + ZeroDir * RayLen, AxisColor);
    Lines.DrawLine(Buffer, Material, Pivot, Pivot + CurrentDir * RayLen, AxisColor);

    if (Flags.bDrawLimits && Joint.FConfig.FEnableLimit)
    {
        // Limit angles are in the same space as theta (relative to reference zero).
        const vec3 LowerDir = Normalize(RotateAroundAxis(AxisA, Joint.FConfig.FLowerAngle, ZeroDir));
        const vec3 UpperDir = Normalize(RotateAroundAxis(AxisA, Joint.FConfig.FUpperAngle, ZeroDir));
        const u32 LowerColor = (Joint.FLimitState == EHingeLimitState::AtLower) ? LimitActiveColor : LimitColor;
        const u32 UpperColor = (Joint.FLimitState == EHingeLimitState::AtUpper) ? LimitActiveColor : LimitColor;
        Lines.DrawLine(Buffer, Material, Pivot, Pivot + LowerDir * RayLen, LowerColor);
        Lines.DrawLine(Buffer, Material, Pivot, Pivot + UpperDir * RayLen, UpperColor);

        constexpr int ArcCount = 12;
        std::vector<vec3> Arc;
        Arc.reserve(ArcCount);
        const float Span = Joint.FConfig.FUpperAngle - Joint.FConfig.FLowerAngle;
        for (int i = 0; i < ArcCount; ++i)
        {
            const float T = (float)i / (float)(ArcCount - 1);
            const float Angle = Joint.FConfig.FLowerAngle + Span * T;
            Arc.push_back(Pivot + Normalize(RotateAroundAxis(AxisA, Angle, ZeroDir)) * (RayLen * 0.85f));
        }
        Lines.DrawPolyline(Buffer, Material, std::move(Arc), LimitColor, false);
    }

    if (Flags.bDrawMotors && Joint.FEnableMotor)
    {
        if (Joint.FUseTargetAngle)
        {
            const vec3 TargetDir = Normalize(RotateAroundAxis(AxisA, Joint.FTargetAngle, ZeroDir));
            Lines.DrawLine(Buffer, Material, Pivot, Pivot + TargetDir * (RayLen * 1.1f), MotorColor);
        }
        else
        {
            const float Speed = Joint.FMotorSpeed;
            if (fabsf(Speed) > 1e-4f)
            {
                const float Sign = (Speed >= 0.f) ? 1.f : -1.f;
                Buffer->DrawDebugArrow(Material, Pivot, AxisA * Sign, Min(fabsf(Speed) * 0.1f, 0.5f), MotorColor);
            }
        }
    }

    if (Flags.bDrawImpulses)
    {
        DrawImpulseArrow(Buffer, Material, Pivot, Joint.FPointImpulse, Flags.ImpulseArrowScale, ImpulseColor);
        Buffer->DrawDebugArrow(Material, Pivot, AxisA, fabsf(Joint.FMotorImpulse) * Flags.ImpulseArrowScale, MotorColor);
        if (Joint.FLimitState != EHingeLimitState::Inactive)
        {
            const vec3 LimitAxis = (Joint.FLimitState == EHingeLimitState::AtLower) ? AxisA : Invert(AxisA);
            Buffer->DrawDebugArrow(Material, Pivot, LimitAxis, fabsf(Joint.FLimitImpulse) * Flags.ImpulseArrowScale, LimitActiveColor);
        }
    }
}
} // namespace

void PhysicsJointsManager::DebugDrawJoints()
{
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bDrawJoints, false, "Draw Joints", "Physics/Joints");
    if (!bDrawJoints)
        return;

    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bDrawJointLimits, true, "Draw Limits", "Physics/Joints");
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bDrawJointMotors, true, "Draw Motors", "Physics/Joints");
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bDrawJointImpulses, false, "Draw Impulses", "Physics/Joints");
    ADJUSTABLE_DEBUG_PARAMETER_SINGLE(jointImpulseArrowScale, 0.1f, "Impulse Arrow Scale", "Physics/Joints", 0.f, 2.f);

    JointDebugDrawFlags Flags;
    Flags.bDrawLimits = bDrawJointLimits;
    Flags.bDrawMotors = bDrawJointMotors;
    Flags.bDrawImpulses = bDrawJointImpulses;
    Flags.ImpulseArrowScale = jointImpulseArrowScale;

    Rendering::DrawCommandBuffer* Buffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::DEBUG_PASS);
    const u32 CamId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
    Camera* camera = CameraManager::Instance().GetCamera(CamId);
    Buffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio()));
    const Rendering::MaterialInstanceHandle Material = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");

    static const u32 AnchorColor = ColorUtils::ConvertToU32(vec4(0.f, 1.f, 1.f, 1.f));
    static const u32 AxisColor = ColorUtils::ConvertToU32(vec4(1.f, 1.f, 0.f, 1.f));
    static const u32 LimitColor = ColorUtils::ConvertToU32(vec4(1.f, 0.55f, 0.1f, 1.f));
    static const u32 LimitActiveColor = ColorUtils::ConvertToU32(vec4(1.f, 0.15f, 0.05f, 1.f));
    static const u32 MotorColor = ColorUtils::ConvertToU32(vec4(1.f, 0.f, 1.f, 1.f));
    static const u32 ImpulseColor = ColorUtils::ConvertToU32(vec4(1.f, 1.f, 1.f, 1.f));

    PhysicsEngine& Engine = PhysicsEngine::Instance();
    JointDebugLineScratch Lines; // must outlive Buffer->Submit() — DrawLines keeps raw pointers

    foreachitemconst(Record, FDistanceJoints)
    {
        if (!Record.Joint.FBodyA.IsValid() || !Record.Joint.FBodyB.IsValid())
            continue;
        const RigidBody* BodyA = Engine.GetRigidBody(Record.Joint.FBodyA);
        const RigidBody* BodyB = Engine.GetRigidBody(Record.Joint.FBodyB);
        if (BodyA == nullptr || BodyB == nullptr)
            continue;
        DrawDistanceJointDebug(Buffer, Material, Lines, Record.Joint, BodyA, BodyB, Flags, AnchorColor, ImpulseColor);
    }

    foreachitemconst(Record, FBallJoints)
    {
        if (!Record.Joint.FBodyA.IsValid() || !Record.Joint.FBodyB.IsValid())
            continue;
        const RigidBody* BodyA = Engine.GetRigidBody(Record.Joint.FBodyA);
        const RigidBody* BodyB = Engine.GetRigidBody(Record.Joint.FBodyB);
        if (BodyA == nullptr || BodyB == nullptr)
            continue;
        DrawBallJointDebug(Buffer, Material, Lines, Record.Joint, BodyA, BodyB, Flags, AnchorColor, AxisColor, LimitColor, LimitActiveColor, MotorColor, ImpulseColor);
    }

    foreachitemconst(Record, FHingeJoints)
    {
        if (!Record.Joint.FBodyA.IsValid() || !Record.Joint.FBodyB.IsValid())
            continue;
        const RigidBody* BodyA = Engine.GetRigidBody(Record.Joint.FBodyA);
        const RigidBody* BodyB = Engine.GetRigidBody(Record.Joint.FBodyB);
        if (BodyA == nullptr || BodyB == nullptr)
            continue;
        DrawHingeJointDebug(Buffer, Material, Lines, Record.Joint, BodyA, BodyB, Flags, AnchorColor, AxisColor, LimitColor, LimitActiveColor, MotorColor, ImpulseColor);
    }

    Buffer->Submit();
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(Buffer);
}
#endif

} // namespace Physics
} // namespace ECSEngine
