#pragma once

#include "SaveLoadData.h"

#define IMPLEMENT_SAVELOAD_ABILITIES_IMPL(TYPE)                                                                                                                                    \
    template<>                                                                                                                                                                     \
    void SaveLoad<SavingSystem::SaveChunk, TYPE, true>(SavingSystem::SaveChunk & parChunk, TYPE & parValue)                                                                        \
    {                                                                                                                                                                              \
        parChunk.GetBuffer().WriteGuards(typeid(TYPE).hash_code());                                                                                                                \
        parValue.SaveLoad<SavingSystem::SaveChunk, true>(parChunk);                                                                                                                \
    }                                                                                                                                                                              \
                                                                                                                                                                                   \
    template<>                                                                                                                                                                     \
    void SaveLoad<SavingSystem::ReadChunk, TYPE, false>(SavingSystem::ReadChunk & parChunk, TYPE & parValue)                                                                       \
    {                                                                                                                                                                              \
        u32 id = parChunk.GetBuffer().ReadId();                                                                                                                                    \
        u32 size = parChunk.GetBuffer().ReadSize();                                                                                                                                \
                                                                                                                                                                                   \
        u32 expectedId = typeid(TYPE).hash_code();                                                                                                                                 \
        AlwaysCheckedAssertMsg(id == expectedId, fmt::format("SaveFile is probably corrupted as the id for {} does not match the id we got !", #TYPE).c_str());                    \
        AlwaysCheckedAssertMsg(size == 0, "Got non zero size while reading guards...");                                                                                            \
                                                                                                                                                                                   \
        parValue.SaveLoad<SavingSystem::ReadChunk, false>(parChunk);                                                                                                               \
    }

#define IMPLEMENT_SAVELOAD_ABILITIES_INSTANCIATIONS(TYPE)                                                                                                                          \
    template void TYPE::SaveLoad<SavingSystem::SaveChunk, true>(SavingSystem::SaveChunk & parChunk);                                                                               \
    template void TYPE::SaveLoad<SavingSystem::ReadChunk, false>(SavingSystem::ReadChunk & parChunk);

#define IMPLEMENT_SAVELOAD_ABILITIES(TYPE)                                                                                                                                         \
    IMPLEMENT_SAVELOAD_ABILITIES_IMPL(TYPE)                                                                                                                                        \
    IMPLEMENT_SAVELOAD_ABILITIES_INSTANCIATIONS(TYPE)