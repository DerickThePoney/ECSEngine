#pragma once
#include "Common/Types.h"

namespace ECSEngine
{
template<typename T>
struct Vector2;
using vec2 = Vector2<float>;
using uvec2 = Vector2<u32>;
using ivec2 = Vector2<i32>;

template<typename T>
struct Vector3;
using vec3 = Vector3<float>;
using uvec3 = Vector3<u32>;
using ivec3 = Vector3<i32>;

template<typename T>
struct Vector4;
using vec4 = Vector4<float>;
using uvec4 = Vector4<u32>;
using ivec4 = Vector4<i32>;

}