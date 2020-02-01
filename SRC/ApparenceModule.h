#pragma once
#include "Module.h"
namespace ECSEngine
{
class ApparenceModule final : public Module
{
    DECLARE_MODULE(ApparenceModule);

public:
    ApparenceModule()
        : Module()
    {
    }

private:
};
} // namespace ECSEngine