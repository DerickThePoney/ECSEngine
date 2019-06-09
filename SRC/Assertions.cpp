#include "stdafx.h"

#include "Assertions.h"

#include "dbghelp.h"

#include <boost/stacktrace.hpp>

namespace ECSEngine
{

AssertImplementation::AssertImplementation(const bool shouldLetGo, const std::string& msg)
{
    if (!shouldLetGo)
        Assert(msg);
}

void AssertImplementation::Assert(const std::string& msg)
{
    std::ostringstream sstr;

    sstr << msg << "\n";
    sstr << boost::stacktrace::stacktrace() << "\n";

    OutputDebugString(sstr.str().c_str());

    __debugbreak();
}

} // namespace ECSEngine