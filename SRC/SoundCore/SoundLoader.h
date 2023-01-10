#pragma once
#include "Common/ILoader.h"

namespace ECSEngine
{
class SoundLoader : public ILoader
{
public:
    SoundLoader()
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

private:
};
} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::SoundLoader);