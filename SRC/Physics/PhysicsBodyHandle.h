#pragma once
#include "Common/Types.h"

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