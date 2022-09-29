#include "MathFunctions.h"
#include "Vector.h"

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

}

