#include "stdafx.h"

#include "Triangle.h"

namespace ECSEngine
{

float Triangle2D::Area() const
{
    vec2 ab = B - A;
    vec2 ac = C - A;

    return 0.5f * abs(ab.x * ac.y - ac.x * ab.y);
}

bool Triangle2D::IsDegenerate() const
{
    return abs(Dot(Normalize(B - A), Normalize(C - A))) > 0.99f;
}

} // namespace ECSEngine
