#pragma once

namespace ECSEngine
{
struct MainOptions
{
    bool IsUsingEditor = false;
};

extern MainOptions Options;
extern void ReadMainCommandLine(int argc, char** argv);
} // namespace ECSEngine