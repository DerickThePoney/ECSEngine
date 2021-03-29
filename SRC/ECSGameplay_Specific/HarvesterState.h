#pragma once

namespace ECSEngine
{
namespace HarvesterState
{
enum Type
{
    IDLE = 0,
    GOING_TO_PRODUCER,
    HARVESTING,
    GOING_BACK_TO_COLONY,
    LENGTH
};

const char* GetName(const Type parState);
} // namespace HarvesterState
} // namespace ECSEngine
