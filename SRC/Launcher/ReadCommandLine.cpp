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
        }
#ifndef COMPILE_FINAL
        else if (strcmp(argv[i], "--nodatapack") == 0)
        {
            Options.NoDatapack = true;
        }
#endif
        ++i;
    }
}
} // namespace ECSEngine