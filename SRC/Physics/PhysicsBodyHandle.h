#pragma once
#include "Common/Types.h"

#include <functional>

namespace ECSEngine
{
namespace Physics
{
struct PhysicsBodyHandle
{
    u32 FId = 0xFFFFFFFF;
    u8 FGeneration = 0x00;

    bool IsValid() const { return FId != 0xFFFFFFFF; }
    bool operator<(const PhysicsBodyHandle& other) const { return FId < other.FId; }
    bool operator==(const PhysicsBodyHandle& other) const { return FId == other.FId && FGeneration == other.FGeneration; }
};
} // namespace Physics
} // namespace ECSEngine

namespace std
{
template<>
struct hash<ECSEngine::Physics::PhysicsBodyHandle>
{
    size_t operator()(const ECSEngine::Physics::PhysicsBodyHandle& parHandle) const noexcept
    {
        const u64 packed = (static_cast<u64>(parHandle.FId) << 8) | parHandle.FGeneration;
        return hash<u64>{ }(packed);
    }
};
} // namespace std