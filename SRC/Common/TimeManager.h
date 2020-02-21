#pragma once

namespace ECSEngine
{
namespace TimeManager
{
void Start();
void NewFrame();
void End();

const float FrameDeltaTime();
const float DurationSinceStartRealTime();
const u32 GetFrameNumber();

} // namespace TimeManager
} // namespace ECSEngine