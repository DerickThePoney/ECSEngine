#include "stdafx.h"

#include "Triangle.h"

namespace ECSEngine
{
bool Triangle2D::IsDegenerate() const
{
    return glm::abs(glm::dot(glm::normalize(B - A), glm::normalize(C - A))) > 0.99f;
}

} // namespace ECSEngine
