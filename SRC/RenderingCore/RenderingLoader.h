#pragma once
#include "Common/ILoader.h"

namespace ECSEngine
{
class RenderingLoader : public ILoader
{
public:
    RenderingLoader(const std::string& parApplicationName = "DEFAULT NAME CHANGE IT NOW NOW NOW NOW!!!")
        : ILoader()
        , FApplicationName(parApplicationName)
    {
    }

    template<class Archive>
    void save(Archive& ar) const
    {
        ar(cereal::base_class<ILoader>(this), PROPERTY(ApplicationName), PROPERTY(RepresentationDescriptors));
    }

    template<class Archive>
    void load(Archive& ar)
    {
        ar(cereal::base_class<ILoader>(this), PROPERTY(ApplicationName));
        PROPERTYFIELD(RepresentationDescriptors, "Configuration\\GFXRepresentationDescriptors.json");
    }

protected:
    virtual bool VirtualInitialise() override;

    virtual void VirtualShutdown() override;

private:
    std::string FApplicationName;
    std::string FRepresentationDescriptors;
};
} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::RenderingLoader);
