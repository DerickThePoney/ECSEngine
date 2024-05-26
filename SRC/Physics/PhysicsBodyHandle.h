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
};
} // namespace Physics
} // namespace ECSEngine