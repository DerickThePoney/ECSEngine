#include "stdafx.h"

#include "MainOptions.h"

#include <cstring>

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