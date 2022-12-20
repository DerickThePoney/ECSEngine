#pragma once

namespace ECSEngine
{
class ISoundEngine;

namespace SoundEngine
{
extern ISoundEngine* Get();
void Initialize();
void Shutdown();
} // namespace SoundEngine
} // namespace ECSEngine