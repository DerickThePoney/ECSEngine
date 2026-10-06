#pragma once
#include "MatrixTypes.h"
#include "VectorTypes.h"

#include <math.h>

namespace ECSEngine
{
template<typename T>
inline __forceinline float Dot(const Vector2<T>& A, const Vector2<T>& B)
{
    return A.x * B.x + A.y * B.y;
}

template<typename T>
inline __forceinline float Dot(const Vector3<T>& A, const Vector3<T>& B)
{
    return A.x * B.x + A.y * B.y + A.z * B.z;
}

template<typename T>
inline __forceinline float Dot(const Vector4<T>& A, const Vector4<T>& B)
{
    return A.x * B.x + A.y * B.y + A.z * B.z + A.w * B.w;
}

template<typename T>
inline __forceinline Vector3<T> Cross(const Vector3<T>& A, const Vector3<T>& B)
{
    return Vector3<T>(A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x);
}

template<typename VecType>
inline __forceinline float LengthSq(const VecType& A)
{
    return Dot(A, A);
}

template<typename VecType>
inline __forceinline float Length(const VecType& A)
{
    return sqrtf(LengthSq(A));
}

template<typename VecType>
inline __forceinline float ooLength(const VecType& A)
{
    return 1.f / sqrtf(LengthSq(A));
}

template<template<typename> typename VecType>
inline __forceinline VecType<float> Normalize(const VecType<float>& A)
{
    return A * ooLength(A);
}

template<template<typename> typename VecType>
inline __forceinline VecType<float> Lerp(const VecType<float>& A, const VecType<float>& B, const float t)
{
    return A * (1.f - t) + B * t;
}

template<typename T>
inline __forceinline Vector2<T> VecMax(const Vector2<T>& A, const Vector2<T>& B)
{
    Vector2<T> res;
    res.x = Max(A.x, B.x);
    res.y = Max(A.y, B.y);

    return res;
}

template<typename T>
inline __forceinline Vector3<T> VecMax(const Vector3<T>& A, const Vector3<T>& B)
{
    Vector3<T> res;
    res.x = Max(A.x, B.x);
    res.y = Max(A.y, B.y);
    res.z = Max(A.z, B.z);

    return res;
}

template<typename T>
inline __forceinline Vector4<T> VecMax(const Vector4<T>& A, const Vector4<T>& B)
{
    Vector4<T> res;
    res.x = Max(A.x, B.x);
    res.y = Max(A.y, B.y);
    res.z = Max(A.z, B.z);
    res.w = Max(A.w, B.w);

    return res;
}

template<typename T>
inline __forceinline Vector2<T> VecMin(const Vector2<T>& A, const Vector2<T>& B)
{
    Vector2<T> res;
    res.x = Min(A.x, B.x);
    res.y = Min(A.y, B.y);

    return res;
}

template<typename T>
inline __forceinline Vector3<T> VecMin(const Vector3<T>& A, const Vector3<T>& B)
{
    Vector3<T> res;
    res.x = Min(A.x, B.x);
    res.y = Min(A.y, B.y);
    res.z = Min(A.z, B.z);

    return res;
}

template<typename T>
inline __forceinline Vector4<T> VecMin(const Vector4<T>& A, const Vector4<T>& B)
{
    Vector4<T> res;
    res.x = Min(A.x, B.x);
    res.y = Min(A.y, B.y);
    res.z = Min(A.z, B.z);
    res.w = Min(A.w, B.w);

    return res;
}

template<typename T>
inline __forceinline Vector3<T> Invert(const Vector3<T>& A)
{
    return -1.f * A;
}

template<typename T>
inline __forceinline Vector4<T> Invert(const Vector4<T>& A)
{
    vec4 m1(-1.f);
    m1.w = 1.f;
    return A * m1;
}

template<typename T>
inline __forceinline Vector2<T> Abs(const Vector2<T>& A)
{
    Vector2<T> res;
    res.x = fabs(A.x);
    res.y = fabs(A.y);

    return res;
}

template<typename T>
inline __forceinline Vector3<T> Abs(const Vector3<T>& A)
{
    Vector3<T> res;
    res.x = fabs(A.x);
    res.y = fabs(A.y);
    res.z = fabs(A.z);

    return res;
}

template<typename T>
inline __forceinline Vector4<T> Abs(const Vector4<T>& A)
{
    Vector4<T> res;
    res.x = fabs(A.x);
    res.y = fabs(A.y);
    res.z = fabs(A.z);
    res.w = fabs(A.w);

    return res;
}

mat3 MultTranspose(const vec3& A, const vec3& B_T);

bool IsNan(const vec2& A);

bool IsNan(const vec3& A);

bool IsNan(const vec4& A);

} // namespace ECSEngine