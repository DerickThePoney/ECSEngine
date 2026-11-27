#pragma once
#include "VectorTypes.h"

namespace ECSEngine
{
constexpr double Pi_dbl()
{
    return 3.14159265358979323846264338327950288;
}

constexpr float Pi()
{
    return float(Pi_dbl());
}

constexpr float Radians(const float parAngleDegree)
{
    return parAngleDegree * Pi() / 180.f;
}

vec3 Radians(const vec3& parV);
float Degree(const float parAngleRadian);
vec3 Degree(const vec3& parV);

template<typename T>
T Min(T a, T b)
{
    return (a <= b) ? a : b;
}

template<typename T>
T Max(T a, T b)
{
    return (a >= b) ? a : b;
}

template<typename T>
T Clamp(T a, T m, T M)
{
    return Min(Max(a, m), M);
}

template<typename T>
T Clamp01(T a)
{
    return Min(Max(a, 0.f), 1.f);
}

vec3 Clamp(const vec3& a, const vec3& m, const vec3& M);

float Atan2(float x, float y);
double Atan2(double x, double y);
float Cos(float x);
double Cos(double x);
float Sin(float x);
double Sin(double x);
bool IsNan(float x);

inline float Sign(float a)
{
    return (a >= 0.f) ? 1.f : -1.f;
}
} // namespace ECSEngine