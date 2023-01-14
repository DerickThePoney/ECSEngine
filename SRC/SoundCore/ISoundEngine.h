#pragma once
#include "Common/Types.h"
#include "Math/VectorTypes.h"

namespace ECSEngine
{

enum class ESoundGroup : u8;
struct SoundHandle;
struct SoundDescriptor;

class ISoundEngine
{
public:
    virtual void Initialize(bool bNoSound) = 0;
    virtual void Shutdown() = 0;

    virtual void TogglePause() = 0;
    virtual void SetVolumeLinear(ESoundGroup SoundGroup, float volume) = 0;

    virtual SoundHandle PlaySoundFromDescriptor(const SoundDescriptor& parDescriptor) = 0;
    virtual bool IsPlaying(const SoundHandle& parSoundHandle) const = 0;
    virtual void StopSound(const SoundHandle& parSoundHandle) = 0;
    virtual float GetSoundDuration(const SoundDescriptor& parDescriptor) = 0;
    virtual float GetSoundDuration(const SoundHandle& parSoundHandle) = 0;

    virtual void SetSoundPosition(const SoundHandle& parSoundHandle, const vec3& parPosition) = 0;
    virtual void SetSoundVelocity(const SoundHandle& parSoundHandle, const vec3& parVelocity) = 0;

    virtual void SetListenerPosition(const vec3& parPosition) = 0;
    virtual void SetListenerVelocity(const vec3& parVelocity) = 0;
    virtual void SetListenerForwardDirection(const vec3& parForwardDirection) = 0;
    virtual void SetListenerUpDirection(const vec3& parUpDirection) = 0;
    virtual void SetListenerCone(float InnerAngle, float OuterAngle, float OutGain) = 0;
};

} // namespace ECSEngine