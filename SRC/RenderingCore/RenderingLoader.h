#pragma once
#include "Application/ILoader.h"

namespace ECSEngine
{
class RenderingLoader : public ILoader
{
public:
    RenderingLoader()
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
    virtual bool VirtualInitialise() override;

    virtual void VirtualShutdown() override;
};
} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::RenderingLoader);