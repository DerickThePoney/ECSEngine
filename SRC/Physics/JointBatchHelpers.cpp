#include "stdafx.h"

#include "JointBatchHelpers.h"

#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{

void ComputeJointConstraintForI(float& M_effi, float& biasi, vec3 ei, float Ci, float invMassA, float invMassB, const mat3& IA, const mat3& IB,
                                const vec3& rA, const vec3& rB, float parDT)
{
    const vec3 rai = Cross(rA, ei);
    const vec3 rbi = Cross(rB, ei);
    const float Ki = invMassA + invMassB + Dot(rai, IA * rai) + Dot(rbi, IB * rbi);
    M_effi = (abs(Ki) < 1.e-3f) ? 0.f : 1.f / Ki;
    biasi = (abs(Ci) < JOINT_LINEAR_SLOP) ? 0.f : (JOINT_BAUMGARTE / parDT) * Ci;
}

void ComputeJointImpulseForI(float& PointImpulseI, float& Pi, float v_reli, float M_effi, float biasi)
{
    const float dli = -M_effi * (v_reli + biasi);
    PointImpulseI += dli;
    Pi = dli;
}

quat JointQuatConjugate(const quat& q)
{
    return quat(-q.x, -q.y, -q.z, q.w);
}

vec3 JointQuatToRotVec(quat q)
{
    if (q.w < 0.f)
        q = quat(-q.x, -q.y, -q.z, -q.w);
    const float w = Clamp(q.w, -1.f, 1.f);
    const float sinHalf = sqrtf(Max(0.f, 1.f - w * w));
    const float angle = 2.f * acosf(w);
    if (sinHalf < 1e-4f || angle < JOINT_ANGULAR_SLOP)
        return vec3();
    return vec3(q.x, q.y, q.z) * (angle / sinHalf);
}

void JointComputeBasis(const vec3& a, vec3& b, vec3& c)
{
    if (fabsf(a.x) >= 0.57735027f)
        b = vec3(a.y, -a.x, 0.f);
    else
        b = vec3(0.f, a.z, -a.y);
    b = Normalize(b);
    c = Cross(a, b);
}

float JointUnwrapAngle(float a, float ref)
{
    while (a - ref > Pi())
        a -= 2.f * Pi();
    while (a - ref < -Pi())
        a += 2.f * Pi();
    return a;
}

} // namespace Physics
} // namespace ECSEngine
