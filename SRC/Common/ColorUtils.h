#pragma once

namespace ECSEngine
{
namespace ColorUtils
{
u32 ConvertToU32(const glm::vec4 parColor);
glm::vec4 ConvertToFVEC4(const u32 parColor);
} // namespace ColorUtils
} // namespace ECSEngine
