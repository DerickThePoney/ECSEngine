#pragma once
#include "Common/ILoader.h"

namespace ECSEngine
{
class LoadInitialiseSubLoaders final : public ILoader
{
public:
    template<typename Archive>
    void save(Archive& ar) const
    {
        ar(cereal::base_class<ILoader>(this), PROPERTY(SubLoaders));
    }

    template<typename Archive>
    void load(Archive& ar)
    {
        ar(cereal::base_class<ILoader>(this), PROPERTY(SubLoaders));
    }

protected:
    bool VirtualInitialise() override;
    void VirtualShutdown() override;

private:
    std::vector<std::unique_ptr<ILoader>> FSubLoaders;
};

class LoaderInitialiseCommonResources final : public ILoader
{
public:
    LoaderInitialiseCommonResources()
        : ILoader()
    {
    }

    template<typename Archive>
    void save(Archive& ar) const
    {
        ar(cereal::base_class<ILoader>(this));
        ar(FSceneManagerConfigFile);
    }

    template<typename Archive>
    void load(Archive& ar)
    {
        ar(cereal::base_class<ILoader>(this));
        PROPERTYFIELD(SceneManagerConfigFile, "");
    }

protected:
    bool VirtualInitialise() override;
    void VirtualShutdown() override;

private:
    std::string FSceneManagerConfigFile;
};
} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::LoaderInitialiseCommonResources);
CEREAL_REGISTER_TYPE(ECSEngine::LoadInitialiseSubLoaders);
