#include "stdafx.h"

#include "ConfigurationManager.h"

namespace ECSEngine
{

ConfigurationManager::ConfigurationManager()
{
}

ConfigurationManager::~ConfigurationManager()
{
}

void ConfigurationManager::Init(std::string parFilename)
{
}

void ConfigurationManager::Destroy()
{
}

//------------------------------------------------------
//		Config BLOB
//------------------------------------------------------

IConfigurationBlob::IConfigurationBlob(const std::string& parConfigName)
    : FConfigName(parConfigName)
#ifdef PERFORM_SECURITY_CHECKS
    , FVirtualSaveToFileCalled(false)
    , FVirtualReadFromFileCalled(false)
#endif
{
}

IConfigurationBlob::~IConfigurationBlob()
{
}

void IConfigurationBlob::SaveToFile(const std::string& parName)
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualSaveToFileCalled = false;
#endif

    VirtualSaveToFile(parName);

#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssertMsg(FVirtualSaveToFileCalled, "You forgot the call to the parent! Baaaaad!");
#endif
}

void IConfigurationBlob::ReadFromFile(const std::string& parName)
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualReadFromFileCalled = false;
#endif

    VirtualReadFromFile(parName);

#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssertMsg(FVirtualReadFromFileCalled, "You forgot the call to the parent! Baaaaad!");
#endif
}

void IConfigurationBlob::VirtualSaveToFile(const std::string& parName)
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualSaveToFileCalled = true;
#endif
}

void IConfigurationBlob::VirtualReadFromFile(const std::string& parName)
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualReadFromFileCalled = true;
#endif
}

} // namespace ECSEngine