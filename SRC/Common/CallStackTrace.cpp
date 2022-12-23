#include "stdafx.h"

#include "CallStackTrace.h"

// clang-format off
#include <windows.h>
#include <dbghelp.h>
// clang-format on

#include "CallStack.h"

namespace ECSEngine
{

CallStackTrace::CallStackTrace()
{
    if (!CallStack::AreSymbolsInitialized())
    {
        return;
    }

    HANDLE process = GetCurrentProcess();

    PVOID rawTrace[1024];
    USHORT frameNum = CaptureStackBackTrace(0, 1024, rawTrace, nullptr);
    Frame temp;
    temp.flags |= FrameFlags::HAS_ADDRESS;
    callstackFrames.resize(frameNum - 1);
    for (USHORT i = 1; i < frameNum; ++i)
    {
        temp.frameAddr = reinterpret_cast<Address>(rawTrace[i]);
        callstackFrames[i - 1] = temp;
    }

    char buffer1[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer1;

    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;

    for (auto& frame : callstackFrames)
    {
        DWORD64 displacement;
        DWORD displacement2;
        DWORD64 addr = static_cast<DWORD64>(frame.frameAddr) - 4;
        IMAGEHLP_MODULE64 modInfo;
        IMAGEHLP_LINE64 lineInfo;
        modInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
        lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        if (SymFromAddr(process, addr, &displacement, pSymbol))
        {
            if (pSymbol->Name)
            {
                frame.flags |= FrameFlags::HAS_FUNC_NAME;
                frame.funcName = pSymbol->Name;
            }
        }

        if (SymGetModuleInfo64(process, addr, &modInfo))
        {
            if (modInfo.ImageName)
            {
                frame.flags |= FrameFlags::HAS_MODULE_INFO;
                frame.moduleName = modInfo.ImageName;
            }
        }

        if (SymGetLineFromAddr64(process, addr, &displacement2, &lineInfo))
        {
            if (lineInfo.FileName)
            {
                frame.flags |= FrameFlags::HAS_LINE_INFO;
                frame.fileName = lineInfo.FileName;
                frame.line = lineInfo.LineNumber;
                frame.column = 0; // Column not supported :(
            }
        }
    }
}

void CallStackTrace::PrintToStream(std::ostringstream& oss)
{
    forrange(i, 0, callstackFrames.size())
    {
        const Frame& f = callstackFrames[i];

        oss << "#" << i << ":" << f.funcName << "(" << f.line << "," << f.column << ")"
            << "\n";
    }
}

} // namespace ECSEngine