#pragma once
#include "QuaternionTypes.h"
#include "VectorTypes.h"

namespace ECSEngine
{
float Dot(const quat& parA, const quat& parB);
quat Normalize(const quat& parQ);

quat Lerp(const quat& A, const quat& B, const float t);

quat AddVectorToQuaternion(const quat& inQuat, const vec3& ScaledVector);
} // namespace ECSEngine
