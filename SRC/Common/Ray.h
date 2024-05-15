#pragma once
#include "Math/VectorTypes.h"
namespace ECSEngine
{
template<typename T>
struct Ray
{
    Ray() { }
    Ray(const T parOrigin, const T parDirection)
        : FOrigin(parOrigin)
        , FDirection(parDirection)
    {
    }
    T FOrigin = T(0.f);
    T FDirection = T(1.f);
};

using Ray2D = Ray<vec2>;
using Ray3D = Ray<vec3>;

static_assert(std::is_trivially_copyable<Ray2D>(), "Ray2D must trivially copyable");
static_assert(std::is_trivially_copyable<Ray3D>(), "Ray3D must trivially copyable");
} // namespace ECSEngine
