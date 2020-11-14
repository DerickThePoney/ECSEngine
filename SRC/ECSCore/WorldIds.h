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
    RESOURCE_PROD,
    PEONS,
    LENGTH
};

const char* GetName(const Type parWorld);
} // namespace Worlds
} // namespace ECSEngine
