#pragma once

namespace ECSEngine
{
struct SoundHandle
{
    bool IsPlaying() const;
    bool IsValid() const;

    u8 FSoundID = (u8)-1;
    u8 FSoundGeneration = (u8)-1;
};
} // namespace ECSEngine