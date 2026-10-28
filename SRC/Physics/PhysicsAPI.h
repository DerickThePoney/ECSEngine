#pragma once

namespace ECSEngine
{
namespace Physics
{
struct PhysicsBodyHandle;
struct PhysicsBodyConfig;
struct PhysicsEngineConfiguration;
struct ContactEvent;
struct PhysicsJointHandle;
struct DistanceJointConfig;
struct BallJointConfig;
struct HingeJointConfig;
class PhysicsCollisionPresetManager;
class PhysicsContactEventManager;

// Init and shutdown
void InitializePhysics(const std::string& ConfigurationFilename);
void SetConfig(const PhysicsEngineConfiguration& Config);
const PhysicsEngineConfiguration& GetConfig();
PhysicsCollisionPresetManager& GetCollisionPresetManager();
PhysicsContactEventManager& GetContactEventManager();
void ShutdownPhysics();

// Physics update
void UpdatePhysics();
// TODO BACKWARD UPDATE

// Body creation/destruction
const PhysicsBodyHandle CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig);
bool DestroyPhysicsBody(const PhysicsBodyHandle& Handle);

// Body manipulation
bool SetBodyPosition(const PhysicsBodyHandle& Handle, const vec3& Position);
bool SetBodyVelocity(const PhysicsBodyHandle& Handle, const vec3& Velocity);
bool SetBodyOrientation(const PhysicsBodyHandle& Handle, const quat& Orientation);
bool SetBodyRotationVelocity(const PhysicsBodyHandle& Handle, const vec3& RotationVelocity);

bool GetBodyPosition(const PhysicsBodyHandle& Handle, vec3& Position);
bool GetBodyVelocity(const PhysicsBodyHandle& Handle, vec3& Velocity);
bool GetBodyOrientation(const PhysicsBodyHandle& Handle, quat& Orientation);
bool GetBodyRotationVelocity(const PhysicsBodyHandle& Handle, vec3& RotationVelocity);

bool AddForceToBody(const PhysicsBodyHandle& Handle, const vec3& Force, bool bTreatAsAcceleration);
bool AddForceAtPointToBody(const PhysicsBodyHandle& Handle, const vec3& Force, const vec3& Point, bool bTreatPointAsLocalCoord, bool bTreatAsAcceleration);
bool AddTorqueToBody(const PhysicsBodyHandle& Handle, const vec3& Torque);

bool AddImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, bool bTreatAsVelocityChange);
bool AddImpulseAtPointToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, const vec3& Point, bool bTreatPointAsLocalCoord, bool bTreatAsVelocityChange);
bool AddRotationImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& RotationImpulse, bool bTreatAsRotationVelocityChange);

// Joints Creation and Removal
const PhysicsJointHandle CreateDistanceJoint(const PhysicsBodyHandle& HandleA, const PhysicsBodyHandle& HandleB, const DistanceJointConfig& Config);
const PhysicsJointHandle CreateBallJoint(const PhysicsBodyHandle& HandleA, const PhysicsBodyHandle& HandleB, const BallJointConfig& Config);
const PhysicsJointHandle CreateHingeJoint(const PhysicsBodyHandle& HandleA, const PhysicsBodyHandle& HandleB, const HingeJointConfig& Config);

void DestroyJoint(const PhysicsJointHandle& JointHandle);

// Joint motors
void SetHingeMotor(const PhysicsJointHandle& JointHandle, bool bEnable, float MotorSpeed, float MaxTorque);
void SetHingeTargetAngle(const PhysicsJointHandle& JointHandle, bool bEnable, float TargetAngle, float Stiffness, float MaxTorque);

#ifdef WITH_VISUAL_DEBUG
// Debugs
void DrawDebugs();
#endif
} // namespace Physics
} // namespace ECSEngine