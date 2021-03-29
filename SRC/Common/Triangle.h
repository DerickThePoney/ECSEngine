#pragma once

namespace ECSEngine
{
class Triangle2D
{
public:
    Triangle2D() { }
    Triangle2D(const glm::vec2 parA, const glm::vec2 parB, const glm::vec2 parC)
        : A(parA)
        , B(parB)
        , C(parC)
    {
    }

    float Area() const;
    bool IsDegenerate() const;

    glm::vec2 A = glm::vec2(0.f);
    glm::vec2 B = glm::vec2(0.f);
    glm::vec2 C = glm::vec2(0.f);
};
} // namespace ECSEngine
