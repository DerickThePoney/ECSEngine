#pragma once
#include "Singleton.h"

namespace ECSEngine
{
class ConfigurationManager final : public Singleton<ConfigurationManager>
{
public:
    ConfigurationManager();
    virtual ~ConfigurationManager();

    void Init(std::string parFilename);
    void Destroy();
};
} // namespace ECSEngine
