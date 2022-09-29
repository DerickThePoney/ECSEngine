#pragma once
#include "MatrixTypes.h"
#include "QuaternionTypes.h"
#include "Vector.h"

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

    inline bool operator==(const Quaternionf& parA) const { return parA.x == x && parA.y == y && parA.z == z && parA.w == w; }

    inline bool operator!=(const Quaternionf& parA) const { return !(*this == parA); }

    inline quat operator*(const float parA) const { return quat(x * parA, y * parA, z * parA, w * parA); }
    quat operator*(const quat& parA) const;
    void operator*=(const quat& parA);
    inline quat operator+(const quat& parA) const { return quat(x + parA.x, y + parA.y, z + parA.z, w + parA.w); }

    explicit operator mat4() const;

    static Quaternionf FromMat4(const mat4& parMatrix);

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    float w = 0.f;
};

inline quat operator*(const quat& parA, const quat& parB)
{
    quat res = parA;
    res *= parB;
    return res;
}
} // namespace ECSEngine