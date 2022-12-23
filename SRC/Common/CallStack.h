#pragma once
namespace ECSEngine
{
namespace CallStack
{
void InitializeSymbols();
void CleanupSymbols();
bool AreSymbolsInitialized();
} // namespace CallStack
} // namespace ECSEngine