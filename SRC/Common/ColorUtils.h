#pragma once

namespace ECSEngine
{
namespace ColorUtils
{
u32 ConvertToU32(const vec4 parColor);
u32 FromRGBA(const u8 r, const u8 g, const u8 b, const u8 a);
vec4 ConvertToFVEC4(const u32 parColor);
} // namespace ColorUtils
} // namespace ECSEngine
