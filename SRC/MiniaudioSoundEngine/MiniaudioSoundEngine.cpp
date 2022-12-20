#include "stdafx.h"

#include "SoundCore/ISoundEngine.h"
#include "SoundCore/SoundGroups.h"
#include "SoundCore/SoundHandle.h"
#include "miniaudio.h"

namespace ECSEngine
{
class MiniAudioSoundEngine final : public ISoundEngine
{
public:
    ~MiniAudioSoundEngine();
    void Initialize(bool bNoSound) override;
    void Shutdown() override;

    void TogglePause() override;
    void SetVolumeLinear(ESoundGroup parSoundGroup, float parVolume);

    SoundHandle PlaySoundFromDescriptor(const SoundDescriptor& parDescriptor) override;
    bool IsPlaying(const SoundHandle& parSoundHandle) const override;
    float GetSoundDuration(const SoundDescriptor& parDescriptor) override;
    float GetSoundDuration(const SoundHandle& parSoundHandle) override;

    void SetSoundPosition(const SoundHandle& parSoundHandle, const vec3& parPosition) override;
    void SetSoundVelocity(const SoundHandle& parSoundHandle, const vec3& parVelocity) override;

    void SetListenerPosition(const vec3& parPosition) override;
    void SetListenerVelocity(const vec3& parVelocity) override;
    void SetListenerForwardDirection(const vec3& parForwardDirection) override;
    void SetListenerUpDirection(const vec3& parUpDirection) override;
    void SetListenerCone(float InnerAngle, float OuterAngle, float OutGain) override;

private:
    ma_sound* GetNextSoundObject(SoundHandle& handle);

private:
    struct ma_customVFS
    {
        ma_vfs_callbacks cb;
    };

    ma_customVFS VFS;

    ma_context FContext;
    ma_engine FEngine;

    ma_sound_group* FSoundGroups[(unsigned int)ESoundGroup::LENGTH];

    std::vector<ma_sound*> FSounds; // TODO MAKE STRUCT THAT IS POOLALLOCATED
    std::vector<SoundHandle> FHandles;
    bool FHasShutdown = false;
    bool FPaused = false;
};

MiniAudioSoundEngine::~MiniAudioSoundEngine()
{
    if (!FHasShutdown)
        Shutdown();
}

void MiniAudioSoundEngine::Initialize(bool bNoSound)
{
}

void MiniAudioSoundEngine::Shutdown()
{
    FHasShutdown = true;
}

void MiniAudioSoundEngine::TogglePause()
{
}

void MiniAudioSoundEngine::SetVolumeLinear(ESoundGroup parSoundGroup, float parVolume)
{
}

SoundHandle MiniAudioSoundEngine::PlaySoundFromDescriptor(const SoundDescriptor& parDescriptor)
{
    return SoundHandle();
}

bool MiniAudioSoundEngine::IsPlaying(const SoundHandle& parSoundHandle) const
{
    return false;
}

float MiniAudioSoundEngine::GetSoundDuration(const SoundDescriptor& parDescriptor)
{
    return 0.f;
}

float MiniAudioSoundEngine::GetSoundDuration(const SoundHandle& parSoundHandle)
{
    return 0.f;
}

void MiniAudioSoundEngine::SetSoundPosition(const SoundHandle& parSoundHandle, const vec3& parPosition)
{
}

void MiniAudioSoundEngine::SetSoundVelocity(const SoundHandle& parSoundHandle, const vec3& parVelocity)
{
}

void MiniAudioSoundEngine::SetListenerPosition(const vec3& parPosition)
{
}

void MiniAudioSoundEngine::SetListenerVelocity(const vec3& parVelocity)
{
}

void MiniAudioSoundEngine::SetListenerForwardDirection(const vec3& parForwardDirection)
{
}

void MiniAudioSoundEngine::SetListenerUpDirection(const vec3& parUpDirection)
{
}

void MiniAudioSoundEngine::SetListenerCone(float InnerAngle, float OuterAngle, float OutGain)
{
}

ma_sound* MiniAudioSoundEngine::GetNextSoundObject(SoundHandle& handle)
{
    return nullptr;
}

namespace SoundEngine
{
std::unique_ptr<MiniAudioSoundEngine> gMiniAudioEngine = nullptr;
ISoundEngine* Get()
{
    if (gMiniAudioEngine == nullptr)
    {
        gMiniAudioEngine.reset(new MiniAudioSoundEngine());
    }

    return gMiniAudioEngine.get();
}
} // namespace SoundEngine
} // namespace ECSEngine