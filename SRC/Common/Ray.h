#pragma once
namespace ECSEngine
{
struct Ray
{
    Ray() { }
    Ray(const glm::vec3 parOrigin, const glm::vec3 parDirection)
        : FOrigin(parOrigin)
        , FDirection(parDirection)
    {
    }
    glm::vec3 FOrigin = glm::vec3(0.f);
    glm::vec3 FDirection = glm::vec3(0.f, 0.f, 1.f);
};

static_assert(std::is_trivially_copyable<Ray>(), "Ray must trivially copyable");
} // namespace ECSEngine
