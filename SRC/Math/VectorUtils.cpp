#include "VectorUtils.h"

#include "MathFunctions.h"
#include "Matrix.h"
#include "Vector.h"

ECSEngine::mat3 ECSEngine::MultTranspose(const vec3& A, const vec3& B_T)
{
    return mat3(A.x * B_T, A.y * B_T, A.z * B_T);
}

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
