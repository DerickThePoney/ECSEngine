#pragma once

#include "Common/Types.h"

#include <functional>

namespace ECSEngine
{
namespace Physics
{
enum class EJointType : u8
{
    DISTANCE,
    BALL,
    HINGE
};

struct PhysicsJointHandle
{
    u16 FIndex = 0xFFFF;
    u8 FType = 0xFF;
    u8 FGeneration = 0;

    EJointType GetTypeAsEnum() const { return (EJointType)FType; }

    bool IsValid() const { return FIndex != 0xFFFF; }
    bool operator==(const PhysicsJointHandle& o) const { return FIndex == o.FIndex && FType == o.FType && FGeneration == o.FGeneration; }
};

} // namespace Physics
} // namespace ECSEngine

namespace std
{
template<>
struct hash<ECSEngine::Physics::PhysicsJointHandle>
{
    size_t operator()(const ECSEngine::Physics::PhysicsJointHandle& h) const noexcept
    {
        const u64 packed = (static_cast<u64>(h.FIndex) << 16) | (static_cast<u64>(h.FType) << 8) | h.FGeneration;
        return hash<u64>{ }(packed);
    }
};
} // namespace std
