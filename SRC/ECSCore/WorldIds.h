#pragma once
namespace ECSEngine
{
namespace Worlds
{
enum Type
{
    STANDARD = 0,
    COLONY,
    CAMERA,
    LENGTH
};

const char* GetName(const Type parWorld);
} // namespace Worlds
} // namespace ECSEngine
