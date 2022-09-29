#include "VectorUtils.h"
#include "Vector.h"
#include "MathFunctions.h"



bool ECSEngine::IsNan(const vec2& A)
{
    return IsNan(A.x) || IsNan(A.y);
}

bool ECSEngine::IsNan(const vec4& A)
{
    return IsNan(A.x) || IsNan(A.y) || IsNan(A.z) || IsNan(A.w);
}

bool ECSEngine::IsNan(const vec3& A)
{
    return IsNan(A.x) || IsNan(A.y) || IsNan(A.z);
}
