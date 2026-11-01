#pragma once
#include "Common/ILoader.h"

namespace ECSEngine
{
class OrbitalLoader : public ILoader
{
public:
    OrbitalLoader()
        : ILoader()
    {
    }

    template<class Archive>
    void save(Archive& ar) const
    {
        ar(cereal::base_class<ILoader>(this));
    }

    template<class Archive>
    void load(Archive& ar)
    {
        ar(cereal::base_class<ILoader>(this));
    }

protected:
    bool VirtualInitialise() override;
    void VirtualShutdown() override;
};
} // namespace ECSEngine
