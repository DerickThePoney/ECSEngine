#include "stdafx.h"

#include "Assertions.h"

#include "Logger.h"

// clang-format off
#include <Windows.h>
#include <dbghelp.h>
// clang-format on

#include "CallStackTrace.h"

namespace ECSEngine
{

AssertImplementation::AssertImplementation(const bool shouldLetGo, const char* msg, const char* additionalMessage)
{
    if (!shouldLetGo)
        Assert(msg, additionalMessage);
}

void AssertImplementation::Assert(const char* msg, const char* additionalMessage)
{
    std::ostringstream sstr;
    CallStackTrace trace;

    sstr << msg << "\n";
    if (additionalMessage != nullptr)
        sstr << additionalMessage << "\n";

    trace.PrintToStream(sstr);

    OutputDebugStringA(sstr.str().c_str());
    LOG_ERROR(sstr.str());
    std::cout << sstr.str() << "\n";

    __debugbreak();
}

} // namespace ECSEngine
