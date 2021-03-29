#pragma once
#include "Singleton.h"

namespace ECSEngine
{

class IConfigurationBlob
{
protected:
    IConfigurationBlob(const std::string& parConfigName);
    virtual ~IConfigurationBlob();

public:
    void SaveToFile(const std::string& parName);
    void ReadFromFile(const std::string& parName);

private:
    virtual void VirtualSaveToFile(const std::string& parName);
    virtual void VirtualReadFromFile(const std::string& parName);

private:
    std::string FConfigName;

#ifdef PERFORM_SECURITY_CHECKS
    bool FVirtualSaveToFileCalled;
    bool FVirtualReadFromFileCalled;
#endif
};

class ConfigurationManager final : public Singleton<ConfigurationManager>
{
public:
    ConfigurationManager();
    virtual ~ConfigurationManager();

    void Init(std::string parFilename);
    void Destroy();
};
} // namespace ECSEngine
