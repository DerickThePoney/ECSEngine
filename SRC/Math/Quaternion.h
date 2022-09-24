#pragma once
#include "Vector.h"
#include "QuaternionTypes.h"


namespace ECSEngine
{
struct alignas(16) Quaternionf
{
    Quaternionf() = default;
    explicit constexpr Quaternionf(float parX, float parY, float parZ, float parW)
        : x(parX)
        , y(parY)
        , z(parZ)
        , w(parW)
    {
    }

    explicit constexpr Quaternionf(const vec4& parV)
        : x(parV.x)
        , y(parV.y)
        , z(parV.z)
        , w(parV.w)
    {
    }

    explicit Quaternionf(const vec3& parAxis, const float parAngle);


    inline bool operator==(const Quaternionf& parA) { return parA.x == x && parA.y == y && parA.z == z && parA.w == w; }

    inline bool operator!=(const Quaternionf& parA) { return !(*this == parA); }

    inline quat operator*(const float parA) { return quat(x * parA, y * parA, z * parA, w * parA); }

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    float w = 0.f;
};

inline quat operator*(const quat& parQ, const float parA)
{
    return quat(parQ.x * parA, parQ.y * parA, parQ.z * parA, parQ.w * parA);
}
}