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

void InitializeSymbols()
{
    HANDLE process = GetCurrentProcess();
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
    HANDLE process = GetCurrentProcess();
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
