#pragma once
#include "Application/ILoader.h"

namespace ECSEngine
{
class ECSLoader : public ILoader
{
public:
    ECSLoader(const std::string& parTemplatesFile = "Dummy")
        : ILoader()
        , FEntityTemplatesFile(parTemplatesFile)
        , FFontsFile(parTemplatesFile)
    {
    }

    template<class Archive>
    void save(Archive& ar) const
    {
        ar(cereal::base_class<ILoader>(this), PROPERTY(EntityTemplatesFile), PROPERTY(FontsFile));
    }

    template<class Archive>
    void load(Archive& ar)
    {
        ar(cereal::base_class<ILoader>(this), PROPERTY(EntityTemplatesFile));
        PROPERTYFIELD(FontsFile, "dummy");
    }

protected:
    virtual bool VirtualInitialise() override;

    virtual void VirtualShutdown() override;

private:
    std::string FEntityTemplatesFile;
    std::string FFontsFile;
};
} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::ECSLoader);
