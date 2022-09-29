#pragma once
#include <functional>
#include "Vector.h"

namespace ECSEngine
{
// from glm straight
inline void HashCombine(size_t& seed, size_t hash)
{
    hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= hash;
}
}

namespace std
{
template<>
struct hash<ECSEngine::vec2>
{
    size_t operator()(const ECSEngine::vec2& v) const
    {
        size_t seed = 0;
        hash<float> hasher;
        ECSEngine::HashCombine(seed, hasher(v.x));
        ECSEngine::HashCombine(seed, hasher(v.y));
        return seed;
    }
};
}