#pragma once

namespace ECSEngine
{
template<typename T>
struct Segment
{
    Segment()
        : Start(T(0.f))
        , End(T(0.f))
    {
    }

    Segment(const T& parStart, const T& parEnd)
        : Start(parStart)
        , End(parEnd)
    {
    }

    T DirectionNormalized() const { return glm::normalize(End - Start); }
    T Direction() const { return End - Start; }
    T Start;
    T End;
};

using Segment2D = Segment<glm::vec2>;
using Segment3D = Segment<glm::vec3>;

static_assert(std::is_trivially_copyable<Segment2D>(), "Segment2D must trivially copyable");
static_assert(std::is_trivially_copyable<Segment3D>(), "Segment3D must trivially copyable");
} // namespace ECSEngine
