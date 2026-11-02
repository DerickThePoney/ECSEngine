#include "stdafx.h"

#include "MainOptions.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace ECSEngine
{
MainOptions Options;

namespace Configuration
{
const char* AssetsDirectory = "..\\Assets\\Colony";
const char* SharedAssetsDirectory = "..\\Assets\\Shared";
const char* SoundsDirectory = "..\\Assets\\Shared\\Sounds";
const char* AssetsDatapackDirectory = "Assets";
const char* SoundDatapackDirectory = "Sounds";
} // namespace Configuration

namespace
{
bool IsGameName(const char* name)
{
    if (name == nullptr || name[0] == '\0')
        return false;
    for (const char* c = name; *c != '\0'; ++c)
    {
        const bool alphanumeric = (*c >= 'A' && *c <= 'Z') || (*c >= 'a' && *c <= 'z') || (*c >= '0' && *c <= '9');
        if (!alphanumeric)
            return false;
    }
    return true;
}
} // namespace

void ReadMainCommandLine(int argc, char** argv)
{
    u32 i = 1;
    while (i < static_cast<u32>(argc))
    {
        if (strcmp(argv[i], "--editor") == 0)
        {
            Options.IsUsingEditor = true;
        }
#ifndef COMPILE_FINAL
        else if (strcmp(argv[i], "--nodatapack") == 0)
        {
            Options.NoDatapack = true;
        }
        else if (strcmp(argv[i], "--nosound") == 0)
        {
            Options.NoSound = true;
        }
        else if (strcmp(argv[i], "--assets") == 0 && i + 1 < static_cast<u32>(argc))
        {
            Configuration::AssetsDirectory = argv[++i];
            Options.AssetsDirectoryOverridden = true;
        }
        else if (strcmp(argv[i], "--game") == 0)
        {
            if (i + 1 >= static_cast<u32>(argc) || !IsGameName(argv[i + 1]))
            {
                fprintf(stderr, "Expected a game name, for example --game Colony\n");
                exit(1);
            }
            ++i;
            if (!Options.AssetsDirectoryOverridden)
            {
                static std::string gameAssetsDirectory;
                gameAssetsDirectory = std::string("..\\Assets\\") + argv[i];
                Configuration::AssetsDirectory = gameAssetsDirectory.c_str();
                Options.AssetsDirectoryOverridden = true;
            }
        }
        else if (strcmp(argv[i], "--shared-assets") == 0 && i + 1 < static_cast<u32>(argc))
        {
            Configuration::SharedAssetsDirectory = argv[++i];
            Options.SharedAssetsDirectoryOverridden = true;
        }
#endif
        ++i;
    }
}
} // namespace ECSEngine