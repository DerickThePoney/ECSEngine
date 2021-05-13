#include "stdafx.h"

#include "ColorUtils.h"

namespace ECSEngine
{
namespace ColorUtils
{

u32 ConvertToU32(const glm::vec4 parColor)
{
    const u8 a = (u8)(parColor.w * 255.f);
    const u8 b = (u8)(parColor.z * 255.f);
    const u8 g = (u8)(parColor.y * 255.f);
    const u8 r = (u8)(parColor.x * 255.f);
    return a << 24 | b << 16 | g << 8 | r;
}

glm::vec4 ConvertToFVEC4(const u32 parColor)
{
    const float a = (float)((parColor >> 24) & 0xFF) / 255.f;
    const float b = (float)((parColor >> 16) & 0xFF) / 255.f;
    const float g = (float)((parColor >> 8) & 0xFF) / 255.f;
    const float r = (float)(parColor & 0xFF) / 255.f;
    return glm::vec4(r, g, b, a);
}

} // namespace ColorUtils
} // namespace ECSEngine
