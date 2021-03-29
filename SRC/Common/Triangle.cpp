#include "stdafx.h"

#include "Triangle.h"

namespace ECSEngine
{

float Triangle2D::Area() const
{
    glm::vec2 ab = B - A;
    glm::vec2 ac = C - A;

    return 0.5f * glm::abs(ab.x * ac.y - ac.x * ab.y);
}

bool Triangle2D::IsDegenerate() const
{
    return glm::abs(glm::dot(glm::normalize(B - A), glm::normalize(C - A))) > 0.99f;
}

} // namespace ECSEngine
