#pragma once
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
}