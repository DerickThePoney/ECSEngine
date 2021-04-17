#pragma once

namespace ECSEngine
{
namespace TimeManager
{
void Create();
void NewFrame();
void NewGameplayTick();
void End();

constexpr float GameplayDeltaTime();
const u32 GameplayCurrentTick();
const float FrameDeltaTime();
const float FrameStartTime();
const float DurationSinceStartRealTime();
const u32 GetFrameNumber();

} // namespace TimeManager
} // namespace ECSEngine
