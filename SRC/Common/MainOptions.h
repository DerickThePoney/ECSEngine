#pragma once

namespace ECSEngine
{
struct MainOptions
{
    bool IsUsingEditor = false;
#ifndef COMPILE_FINAL
    bool NoDatapack = false;
#endif
};
extern MainOptions Options;
extern void ReadMainCommandLine(int argc, char** argv);

namespace Configuration
{
extern const char* AssetsDirectory;
extern const char* DatapackDirectory;
} // namespace Configuration

bool InitialiseGlobalCache();
void DestroyGlobalCache();
} // namespace ECSEngine