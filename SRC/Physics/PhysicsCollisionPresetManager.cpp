#include "stdafx.h"

#include "PhysicsCollisionPresetManager.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "PhysicsEngine.h"

#include <fstream>

namespace ECSEngine
{
namespace Physics
{
void PhysicsCollisionPresetManager::Initialize(PhysicsEngine* Engine)
{
    const PhysicsEngineConfiguration& Config = Engine->Config();

    {
        std::string ConfigurationFilename = "Configuration\\PhysicsCollisionPresets.json";
        AssertRelease(PhysicsEngine::HasInstance());
        Resource r(ConfigurationFilename);
        if (GlobalResourceCache::Instance().FCache->FileExists(&r))
        {
            auto handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
            AssertRelease(handle != nullptr);
            ResourceBuffer buff = handle->GetResourceBuffer();
            std::istream istr(&buff, std::istream::in);
            cereal::JSONInputArchive archive(istr);

            archive(NAMEDPROPERTY("PhysicsCollisionPresets", FCollisionPresets));
        }
        else
        {
            std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + ConfigurationFilename);
            cereal::JSONOutputArchive archive(ofstr);
            archive(NAMEDPROPERTY("PhysicsCollisionPresets", FCollisionPresets));
        }
    }
}

#pragma region Presets
std::string PhysicsCollisionPreset::GetCategoryName() const
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    const PhysicsEngineConfiguration& Config = Engine.Config();

    return Config.LayerName(FCollisionCategory);
}
std::vector<std::string> PhysicsCollisionPreset::GetCollisionMaskNames() const
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    const PhysicsEngineConfiguration& Config = Engine.Config();

    std::vector<std::string> MaskNames;

    if (FCollisionMask == 0)
        return MaskNames;

    if (FCollisionMask == ~0u)
    {
        MaskNames.push_back("ALL");
        return MaskNames;
    }

    forrange(i, 0, Config.FNumLayers)
    {
        u32 LayerBit = 1u << i;
        if ((FCollisionMask & LayerBit) == 0)
        {
            continue;
        }

        MaskNames.push_back(Config.LayerName(LayerBit));
    }

    return MaskNames;
}
void PhysicsCollisionPreset::SetCategoryName(std::string& Category)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    const PhysicsEngineConfiguration& Config = Engine.Config();

    FCollisionCategory = Config.LayerBit(Category);
}
void PhysicsCollisionPreset::SetCollisionMaskNames(std::vector<std::string>& Masks)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    const PhysicsEngineConfiguration& Config = Engine.Config();

    if (Masks.empty())
    {
        FCollisionMask = 0;
        return;
    }

    if (Masks.size() == 1 && Masks[0] == "ALL")
    {
        FCollisionMask = ~0u;
        return;
    }

    forrange(i, 0, Masks.size())
    {
        u32 LayerBit = Config.LayerBit(Masks[i]);
        FCollisionMask = FCollisionMask & LayerBit;
    }
}
#pragma endregion Presets
} // namespace Physics
} // namespace ECSEngine
