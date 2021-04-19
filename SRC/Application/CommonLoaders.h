#pragma once
#include "ILoader.h"

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
    }

    template<typename Archive>
    void load(Archive& ar)
    {
        ar(cereal::base_class<ILoader>(this));
    }

protected:
    bool VirtualInitialise() override;
    void VirtualShutdown() override;
};
} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::LoaderInitialiseCommonResources);
CEREAL_REGISTER_TYPE(ECSEngine::LoadInitialiseSubLoaders);
