#pragma once
#include "Common/Types.h"

namespace ECSEngine
{
enum class ESoundGroup : u8
{
    MUSICS = 0,
    EFFECTS,
    LENGTH,
    MASTER
};

namespace SoundGroup
{
char* GetName(ESoundGroup parValue);
}
} // namespace ECSEngine