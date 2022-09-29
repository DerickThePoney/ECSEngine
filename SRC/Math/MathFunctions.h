#pragma once
#include "VectorTypes.h"

namespace ECSEngine
{
constexpr float Pi()
{
    return 3.14159265358979323846264338327950288f;
}

float Radians(const float parAngleDegree);
vec3 Radians(const vec3& parV);
float Degree(const float parAngleRadian);
vec3 Degree(const vec3& parV);

inline float Min(float a, float b)
{
    return (a <= b) ? a : b;
}
inline float Max(float a, float b)
{
    return (a >= b) ? a : b;
}

inline float Clamp(float a, float m, float M)
{
    return Min(Max(a, m), M);
}
} // namespace ECSEngine