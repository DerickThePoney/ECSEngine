#pragma once
namespace ECSEngine
{
enum class EEntityWorlds : u32
{
    STANDARD = 0,
    COLONY,
    CAMERA,
    RESOURCE_PROD,
    PEONS,
    BUILDINGS,
    LENGTH
};

namespace EEntityWorldsHelpers
{
const char* GetName(const EEntityWorlds parWorld);
}
} // namespace ECSEngine
