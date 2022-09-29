#pragma once

namespace ECSEngine
{
class Triangle2D
{
public:
    Triangle2D() { }
    Triangle2D(const vec2 parA, const vec2 parB, const vec2 parC)
        : A(parA)
        , B(parB)
        , C(parC)
    {
    }

    float Area() const;
    bool IsDegenerate() const;

    vec2 A = vec2(0.f);
    vec2 B = vec2(0.f);
    vec2 C = vec2(0.f);
};
} // namespace ECSEngine
