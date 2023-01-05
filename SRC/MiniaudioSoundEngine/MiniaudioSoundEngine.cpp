#include "stdafx.h"

#include "SoundCore/ISoundEngine.h"
#include "SoundCore/SoundGroups.h"
#include "SoundCore/SoundHandle.h"

#define MINIAUDIO_IMPLEMENTATION
#include "MACustomVFSMethods.h"
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
    ma_result result;

    MACustomVFSMethods::CreateVFS();

    if (bNoSound)
    {
        ma_backend backends[] = { ma_backend_null };

        result = ma_context_init(backends, 1, NULL, &FContext);
    }
    else
    {
        result = ma_context_init(NULL, 1, NULL, &FContext);
    }

    AlwaysCheckedAssertMsg(result == MA_SUCCESS, "Unable to init sound context...");
    if (result != MA_SUCCESS)
    {
        MACustomVFSMethods::CloseVFS();
        return;
    }

    // init custom vfs
    VFS.cb.onClose = MACustomVFSMethods::ma_vfs_close;
    VFS.cb.onOpen = MACustomVFSMethods::ma_vfs_open;
    VFS.cb.onRead = MACustomVFSMethods::ma_vfs_read;
    VFS.cb.onSeek = MACustomVFSMethods::ma_vfs_seek;
    VFS.cb.onTell = MACustomVFSMethods::ma_vfs_tell;
    VFS.cb.onInfo = MACustomVFSMethods::ma_vfs_info;

    // init engine
    ma_engine_config config = ma_engine_config_init();
    config.pContext = &FContext;
    config.pResourceManagerVFS = &VFS;

    result = ma_engine_init(&config, &FEngine);
    AlwaysCheckedAssertMsg(result == MA_SUCCESS, "Unable to init sound engine...");
    if (result != MA_SUCCESS)
    {
        ma_context_uninit(&FContext);
        MACustomVFSMethods::CloseVFS();
        return;
    }

    for (int i = 0; i < (int)ESoundGroup::LENGTH; ++i)
    {
        FSoundGroups[i] = new ma_sound_group();
        ma_sound_group_init(&FEngine, 0, NULL, FSoundGroups[i]);
    }

    ma_engine_listener_set_position(&FEngine, 0, 0, 0, 0);
}

void MiniAudioSoundEngine::Shutdown()
{
    for (int i = 0; i < FSounds.size(); ++i)
    {
        ma_sound_uninit(FSounds[i]);
        delete FSounds[i];
    }

    for (int i = 0; i < (int)ESoundGroup::LENGTH; ++i)
    {
        ma_sound_group_uninit(FSoundGroups[i]);
        delete FSoundGroups[i];
    }

    ma_engine_uninit(&FEngine);

    ma_context_uninit(&FContext);

    MACustomVFSMethods::CloseVFS();

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