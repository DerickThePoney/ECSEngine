#pragma once

#include "Math/MathFunctions.h"
#include "Math/MatrixTypes.h"
#include "Math/QuaternionTypes.h"
#include "Math/VectorTypes.h"

namespace ECSEngine
{
namespace Physics
{

static constexpr float JOINT_BAUMGARTE = 0.3f;
static constexpr float JOINT_LINEAR_SLOP = 0.005f;
static constexpr float JOINT_ANGULAR_SLOP = 2.f * Pi() / 180.f;

void ComputeJointConstraintForI(float& M_effi, float& biasi, vec3 ei, float Ci, float invMassA, float invMassB, const mat3& IA, const mat3& IB,
                                const vec3& rA, const vec3& rB, float parDT);

void ComputeJointImpulseForI(float& PointImpulseI, float& Pi, float v_reli, float M_effi, float biasi);

quat JointQuatConjugate(const quat& q);
vec3 JointQuatToRotVec(quat q);
void JointComputeBasis(const vec3& a, vec3& b, vec3& c);
float JointUnwrapAngle(float a, float ref);

} // namespace Physics
} // namespace ECSEngine
