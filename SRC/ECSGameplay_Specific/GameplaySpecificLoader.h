#pragma once
#include "Application/ILoader.h"

namespace ECSEngine
{
class ECSGameplaySpecificLoader : public ILoader
{
public:
    ECSGameplaySpecificLoader(const std::string& parGameplayRulesFile = "Dummy")
        : ILoader()
        , FGameplayRulesFile(parGameplayRulesFile)
    {
    }

    template<class Archive>
    void save(Archive& ar) const
    {
        ar(cereal::base_class<ILoader>(this), PROPERTY(GameplayRulesFile));
    }

    template<class Archive>
    void load(Archive& ar)
    {
        ar(cereal::base_class<ILoader>(this), PROPERTY(GameplayRulesFile));
    }

protected:
    virtual bool VirtualInitialise() override;

    virtual void VirtualShutdown() override;

private:
    std::string FGameplayRulesFile;
};
} // namespace ECSEngine