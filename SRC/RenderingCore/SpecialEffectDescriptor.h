#pragma once
#include "Common/PoolAllocator.h"

namespace ECSEngine
{
struct SoundDescriptor;
struct VisualEffectDescriptor;

struct SpecialEffectDescriptor
{
    DECLARE_POOL_ALLOCATED(SpecialEffectDescriptor);

public:
    void DrawInEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(VFXDescriptor, nullptr);
        PROPERTYFIELD(SoundDescriptor, nullptr);
    }

private:
    std::unique_ptr<VisualEffectDescriptor> FVFXDescriptor;
    std::unique_ptr<SoundDescriptor> FSoundDescriptor;
};

} // namespace ECSEngine