#include "MathFunctions.h"

#include "Vector.h"

#include <cmath>

namespace ECSEngine
{

float Radians(const float parAngleDegree)
{
    return parAngleDegree * Pi() / 180.f;
}

vec3 Radians(const vec3& parV)
{
    return vec3(Radians(parV.x), Radians(parV.y), Radians(parV.z));
}

float Degree(const float parAngleRadian)
{
    return parAngleRadian * 180.f / Pi();
}

vec3 Degree(const vec3& parV)
{
    return vec3(Degree(parV.x), Degree(parV.y), Degree(parV.z));
}

vec3 Clamp(const vec3& a, const vec3& m, const vec3& M)
{
    return vec3(Clamp(a.x, m.x, M.x), Clamp(a.y, m.y, M.y), Clamp(a.z, m.z, M.z));
}

float Atan2(float x, float y)
{
    return atan2f(x, y);
}

float Cos(float x)
{
    return cosf(x);
}

float Sin(float x)
{
    return sinf(x);
}

bool IsNan(float x)
{
    return isnan(x);
}

} // namespace ECSEngine
