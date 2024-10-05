#pragma once
namespace ECSEngine
{
namespace Physics
{
namespace EPhysicsMoveability
{
enum Type
{
    STATIC,
    KINEMATIC,
    PHYICS_ENABLED,
    LENGTH
};

std::string AsString(Type value);
} // namespace EPhysicsMoveability
} // namespace Physics
} // namespace ECSEngine