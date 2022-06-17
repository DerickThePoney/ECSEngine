#pragma once
#include "Macros.h"
#include "Types.h"

namespace ECSEngine
{
namespace MathHelpers
{
FORCEINLINE u32 NextPowerOfTwo(u32 v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}
} // namespace MathHelpers
} // namespace ECSEngine
