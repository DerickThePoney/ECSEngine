#pragma once

namespace ECSEngine
{
namespace RandomNumbers
{
void InitRandomNumberGenerator(const u32 parSeed);
void DestroyRandomNumberGenerator();
float NextFloat();
} // namespace RandomNumbers
} // namespace ECSEngine
