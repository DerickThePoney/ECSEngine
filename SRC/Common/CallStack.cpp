#include "stdafx.h"

#include "CallStack.h"

// clang-format off
#include <windows.h>
#include <dbghelp.h>
// clang-format on

namespace ECSEngine
{
namespace CallStack
{
std::atomic_bool bSymbolsInitialized = false;
#pragma warning(push)
#pragma warning(disable : 4312)
HANDLE GetCurrentProcessHandle()
{
    return (HANDLE)GetCurrentProcessId();
}
#pragma warning(pop)

void InitializeSymbols()
{
    HANDLE process = GetCurrentProcessHandle();
    BOOL result = SymInitialize(process, nullptr, TRUE);
    if (result == 0)
    {
        std::cerr << "[CallStack::InitializeSymbols] (Windows) Failed to initialize symbols (SymInitialize)" << std::endl;
        return;
    }
    bSymbolsInitialized.store(true);
}

void CleanupSymbols()
{
    HANDLE process = (HANDLE)GetCurrentProcessHandle();
    BOOL result = SymCleanup(process);
    if (result == 0)
    {
        std::cerr << "[CallStack::CleanupSymbols] (Windows) Failed to clean symbols (SymCleanup)" << std::endl;
        return;
    }
    bSymbolsInitialized.store(false);
}

bool AreSymbolsInitialized()
{
    return bSymbolsInitialized.load();
}

} // namespace CallStack
} // namespace ECSEngine
