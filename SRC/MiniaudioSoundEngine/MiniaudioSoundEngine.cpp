#include "stdafx.h"

#include "Common/PoolAllocator.h"
#include "MACustomVFSMethods.h"
#include "SoundCore/ISoundEngine.h"
#include "SoundCore/SoundDescriptor.h"
#include "SoundCore/SoundGroups.h"
#include "SoundCore/SoundHandle.h"

#define MINIAUDIO_IMPLEMENTATION
#include "Common/Logger.h"
#include "miniaudio.h"

namespace ECSEngine
{
static constexpr u32 SoundWrapperPoolChunkSize = 128;
struct MASoundWrapper
{
    DECLARE_POOL_ALLOCATED_CUSTOM_CHUNK_SIZE(MASoundWrapper, SoundWrapperPoolChunkSize);

public:
    ~MASoundWrapper() { ma_sound_uninit(&FSound); }
    ma_sound FSound;
};

IMPLEMENT_POOL_ALLOCATED_CUSTOM_CHUNK_SIZE(MASoundWrapper, SoundWrapperPoolChunkSize);

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
    void StopSound(const SoundHandle& parSoundHandle) override;
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
    MASoundWrapper* GetNextSoundObject(SoundHandle& handle);

private:
    struct ma_customVFS
    {
        ma_vfs_callbacks cb;
    };

    ma_customVFS VFS;

    ma_context FContext;
    ma_engine FEngine;

    ma_sound_group* FSoundGroups[(unsigned int)ESoundGroup::LENGTH];

    std::vector<MASoundWrapper*> FSounds;
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
        LOG_SOUND("Unable to init sound context...");
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
        LOG_SOUND("Unable to init sound engine...");
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
    if (FPaused)
        ma_engine_start(&FEngine);
    else
        ma_engine_stop(&FEngine);

    FPaused = !FPaused;
}

void MiniAudioSoundEngine::SetVolumeLinear(ESoundGroup parSoundGroup, float parVolume)
{
    switch (parSoundGroup)
    {
    case ESoundGroup::MUSICS:
    case ESoundGroup::EFFECTS:
    {
        ma_sound_set_volume(FSoundGroups[(int)parSoundGroup], parVolume);
        break;
    }
    case ESoundGroup::MASTER:
    {
        ma_engine_set_volume(&FEngine, parVolume);
        break;
    }
    }
}

SoundHandle MiniAudioSoundEngine::PlaySoundFromDescriptor(const SoundDescriptor& parDescriptor)
{
    ma_result result;

    SoundHandle handle;
    MASoundWrapper* Sound = GetNextSoundObject(handle);

    int flags = MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC;
    if (parDescriptor.FStream)
        flags |= MA_SOUND_FLAG_STREAM;
    if (!parDescriptor.FSpatialized)
        flags |= MA_SOUND_FLAG_NO_SPATIALIZATION;
    if (!parDescriptor.FAllowPitchChanges)
        flags |= MA_SOUND_FLAG_NO_PITCH;

    result = ma_sound_init_from_file(&FEngine, parDescriptor.FFilename.c_str(), flags, FSoundGroups[(unsigned int)parDescriptor.FSoundGroup], NULL, &Sound->FSound);
    AlwaysCheckedAssertMsg(result == MA_SUCCESS, std::format("WARNING: Failed to load sound \"{}\"", parDescriptor.FFilename.c_str()).c_str());
    if (result != MA_SUCCESS)
    {
        LOG_SOUND(std::format("WARNING: Failed to load sound \"{}\"", parDescriptor.FFilename.c_str()).c_str());
        return {};
    }

    ma_sound_set_looping(&Sound->FSound, parDescriptor.FbLoop);

    result = ma_sound_start(&Sound->FSound);
    AlwaysCheckedAssertMsg(result == MA_SUCCESS, std::format("WARNING: Failed to start sound {}", parDescriptor.FFilename.c_str()).c_str());
    if (result != MA_SUCCESS)
    {
        LOG_SOUND(std::format("WARNING: Failed to start sound {}", parDescriptor.FFilename.c_str()).c_str());
        return {};
    }

    return handle;
}

bool MiniAudioSoundEngine::IsPlaying(const SoundHandle& parSoundHandle) const
{
    if (parSoundHandle.FSoundGeneration != FHandles[parSoundHandle.FSoundID].FSoundGeneration)
        return false;
    return !ma_sound_at_end(&FSounds[parSoundHandle.FSoundID]->FSound);
}

void MiniAudioSoundEngine::StopSound(const SoundHandle& parSoundHandle)
{
    if (IsPlaying(parSoundHandle))
    {
        if (parSoundHandle.FSoundGeneration == FHandles[parSoundHandle.FSoundID].FSoundGeneration)
        {
            ma_sound_stop(&FSounds[parSoundHandle.FSoundID]->FSound);
            FHandles[parSoundHandle.FSoundID].FSoundGeneration++;
        }
    }
}

float MiniAudioSoundEngine::GetSoundDuration(const SoundDescriptor& parDescriptor)
{
    ma_sound Sound;
    ma_result result;
    result = ma_sound_init_from_file(&FEngine, parDescriptor.FFilename.c_str(), 0, NULL, NULL, &Sound);
    AlwaysCheckedAssertMsg(result == MA_SUCCESS, std::format("WARNING: Failed to load sound \"{}\"", parDescriptor.FFilename.c_str()).c_str());
    if (result != MA_SUCCESS)
    {
        LOG_SOUND(std::format("WARNING: Failed to load sound \"{}\"", parDescriptor.FFilename.c_str()).c_str());
        return -1.f;
    }

    float res = 0.f;
    result = ma_sound_get_length_in_seconds(&Sound, &res);
    AlwaysCheckedAssertMsg(result == MA_SUCCESS, std::format("WARNING: Failed to get length of sound \"{}\"", parDescriptor.FFilename.c_str()).c_str());
    if (result != MA_SUCCESS)
    {
        LOG_SOUND(std::format("WARNING: Failed to get length of sound \"{}\"", parDescriptor.FFilename.c_str()).c_str());
        return -1.f;
    }

    ma_sound_uninit(&Sound);

    return res;
}

float MiniAudioSoundEngine::GetSoundDuration(const SoundHandle& parSoundHandle)
{
    if (!parSoundHandle.IsValid() || (parSoundHandle.FSoundGeneration != FHandles[parSoundHandle.FSoundID].FSoundGeneration))
        return -1.f;

    float res = 0.f;
    ma_result result = ma_sound_get_length_in_seconds(&FSounds[parSoundHandle.FSoundID]->FSound, &res);
    AlwaysCheckedAssertMsg(result == MA_SUCCESS, "WARNING: Failed to get length of sound");
    if (result != MA_SUCCESS)
    {
        LOG_SOUND("WARNING: Failed to get length of sound");
        return -1.f;
    }

    return res;
}

void MiniAudioSoundEngine::SetSoundPosition(const SoundHandle& parSoundHandle, const vec3& parPosition)
{
    if (!parSoundHandle.IsValid() || (parSoundHandle.FSoundGeneration != FHandles[parSoundHandle.FSoundID].FSoundGeneration))
        return;

    ma_sound_set_position(&FSounds[parSoundHandle.FSoundID]->FSound, parPosition.x, parPosition.y, parPosition.z);
}

void MiniAudioSoundEngine::SetSoundVelocity(const SoundHandle& parSoundHandle, const vec3& parVelocity)
{
    if (!parSoundHandle.IsValid() || (parSoundHandle.FSoundGeneration != FHandles[parSoundHandle.FSoundID].FSoundGeneration))
        return;

    ma_sound_set_velocity(&FSounds[parSoundHandle.FSoundID]->FSound, parVelocity.x, parVelocity.y, parVelocity.z);
}

void MiniAudioSoundEngine::SetListenerPosition(const vec3& parPosition)
{
    ma_engine_listener_set_position(&FEngine, 0, parPosition.x, parPosition.y, parPosition.z);
}

void MiniAudioSoundEngine::SetListenerVelocity(const vec3& parVelocity)
{
    ma_engine_listener_set_velocity(&FEngine, 0, parVelocity.x, parVelocity.y, parVelocity.z);
}

void MiniAudioSoundEngine::SetListenerForwardDirection(const vec3& parForwardDirection)
{
    ma_engine_listener_set_direction(&FEngine, 0, parForwardDirection.x, parForwardDirection.y, parForwardDirection.z);
}

void MiniAudioSoundEngine::SetListenerUpDirection(const vec3& parUpDirection)
{
    ma_engine_listener_set_world_up(&FEngine, 0, parUpDirection.x, parUpDirection.y, parUpDirection.z);
}

void MiniAudioSoundEngine::SetListenerCone(float InnerAngle, float OuterAngle, float OutGain)
{
    ma_engine_listener_set_cone(&FEngine, 0, InnerAngle, OuterAngle, OutGain);
}

MASoundWrapper* MiniAudioSoundEngine::GetNextSoundObject(SoundHandle& handle)
{
    MASoundWrapper* Result = nullptr;

    for (size_t i = 0; i < FSounds.size(); ++i)
    {
        MASoundWrapper*& SoundInUse = FSounds[i];
        if (ma_sound_at_end(&SoundInUse->FSound) || !ma_sound_is_playing(&SoundInUse->FSound))
        {
            Result = SoundInUse;
            ma_sound_uninit(&Result->FSound);
            FHandles[i].FSoundGeneration++;
            handle = FHandles[i];
            break;
        }
    }

    if (Result == nullptr)
    {
        SoundHandle newHandle;
        newHandle.FSoundID = (uint8_t)FHandles.size();
        newHandle.FSoundGeneration = 0;
        FHandles.emplace_back(std::move(newHandle));
        handle = newHandle;

        FSounds.push_back(new MASoundWrapper());
        Result = FSounds.back();
        printf("Creating new sound object\n");
    }

    return Result;
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