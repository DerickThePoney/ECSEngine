#include "stdafx.h"

#include "Common/MainOptions.h"

namespace ECSEngine
{
MainOptions Options;

void ReadMainCommandLine(int argc, char** argv)
{
    u32 i = 1;
    while (i < argc)
    {
        if (strcmp(argv[i], "--editor") == 0)
        {
            Options.IsUsingEditor = true;
            ++i;
        }
    }
}
} // namespace ECSEngine