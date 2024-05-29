#pragma once
#include "Common/ILoader.h"

namespace ECSEngine
{
class PhysicsLoader : public ILoader
{
public:
    PhysicsLoader()
        : ILoader()
    {
    }

    template<class Archive>
    void save(Archive& ar) const
    {
        ar(cereal::base_class<ILoader>(this));

        ar(FConfigurationFilename);
    }

    template<class Archive>
    void load(Archive& ar)
    {
        ar(cereal::base_class<ILoader>(this));

        PROPERTYFIELD(ConfigurationFilename, "Configuration\\PhysicsConfiguration.json");
    }

protected:
    virtual bool VirtualInitialise() override;

    virtual void VirtualShutdown() override;

private:
    std::string FConfigurationFilename = "Configuration\\PhysicsConfiguration.json";
};
} // namespace ECSEngine
CEREAL_REGISTER_TYPE(ECSEngine::PhysicsLoader);