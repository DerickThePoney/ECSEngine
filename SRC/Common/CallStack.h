#pragma once
typedef void* HANDLE;
namespace ECSEngine
{
namespace CallStack
{
HANDLE GetCurrentProcessHandle();
void InitializeSymbols();
void CleanupSymbols();
bool AreSymbolsInitialized();
} // namespace CallStack
} // namespace ECSEngine