#pragma once

#include "SaveLoadData.h"

#define IMPLEMENT_SAVELOAD_ABILITIES_IMPL(TYPE)                                                                                                                                    \
    void TYPE::SaveLoad(class SavingSystem::SaveChunk& parChunk) { SaveLoad<SavingSystem::SaveChunk, true>(parChunk); }                                                            \
    void TYPE::SaveLoad(class SavingSystem::ReadChunk& parChunk) { SaveLoad<SavingSystem::ReadChunk, false>(parChunk); }                                                           \
                                                                                                                                                                                   \
    template<>                                                                                                                                                                     \
    void SaveLoad<SavingSystem::SaveChunk, TYPE, true>(SavingSystem::SaveChunk & parChunk, TYPE & parValue)                                                                        \
    {                                                                                                                                                                              \
        parChunk.GetBuffer().WriteGuards(typeid(TYPE).hash_code());                                                                                                                \
        parValue.SaveLoad(parChunk);                                                                                                                                               \
    }                                                                                                                                                                              \
                                                                                                                                                                                   \
    template<>                                                                                                                                                                     \
    void SaveLoad<SavingSystem::ReadChunk, TYPE, false>(SavingSystem::ReadChunk & parChunk, TYPE & parValue)                                                                       \
    {                                                                                                                                                                              \
        u32 id = parChunk.GetBuffer().ReadId();                                                                                                                                    \
        u32 size = parChunk.GetBuffer().ReadSize();                                                                                                                                \
                                                                                                                                                                                   \
        u32 expectedId = typeid(TYPE).hash_code();                                                                                                                                 \
        AlwaysCheckedAssertMsg(id == expectedId, std::format("SaveFile is probably corrupted as the id for {} does not match the id we got !", #TYPE).c_str());                    \
        AlwaysCheckedAssertMsg(size == 0, "Got non zero size while reading guards...");                                                                                            \
                                                                                                                                                                                   \
        parValue.SaveLoad(parChunk);                                                                                                                                               \
    }

#define IMPLEMENT_SAVELOAD_ABILITIES_POINTERS_IMPL(TYPE)                                                                                                                           \
    template<>                                                                                                                                                                     \
    void SaveLoad<SavingSystem::SaveChunk, TYPE*, true>(SavingSystem::SaveChunk & parChunk, TYPE * &parValue)                                                                      \
    {                                                                                                                                                                              \
        parChunk.GetBuffer().WriteGuards(typeid(TYPE).hash_code());                                                                                                                \
        parValue->SaveLoad(parChunk);                                                                                                                                              \
    }                                                                                                                                                                              \
                                                                                                                                                                                   \
    template<>                                                                                                                                                                     \
    void SaveLoad<SavingSystem::ReadChunk, TYPE*, false>(SavingSystem::ReadChunk & parChunk, TYPE * &parValue)                                                                     \
    {                                                                                                                                                                              \
        u32 id = parChunk.GetBuffer().ReadId();                                                                                                                                    \
        u32 size = parChunk.GetBuffer().ReadSize();                                                                                                                                \
                                                                                                                                                                                   \
        u32 expectedId = typeid(TYPE).hash_code();                                                                                                                                 \
        AlwaysCheckedAssertMsg(id == expectedId, std::format("SaveFile is probably corrupted as the id for {} does not match the id we got !", #TYPE).c_str());                    \
        AlwaysCheckedAssertMsg(size == 0, "Got non zero size while reading guards...");                                                                                            \
                                                                                                                                                                                   \
        parValue->SaveLoad(parChunk);                                                                                                                                              \
    }

#define IMPLEMENT_SAVELOAD_ABILITIES_INSTANCIATIONS(TYPE)                                                                                                                          \
    template void TYPE::SaveLoad<SavingSystem::SaveChunk, true>(SavingSystem::SaveChunk & parChunk);                                                                               \
    template void TYPE::SaveLoad<SavingSystem::ReadChunk, false>(SavingSystem::ReadChunk & parChunk);

#define IMPLEMENT_SAVELOAD_ABILITIES(TYPE)                                                                                                                                         \
    IMPLEMENT_SAVELOAD_ABILITIES_IMPL(TYPE)                                                                                                                                        \
    IMPLEMENT_SAVELOAD_ABILITIES_INSTANCIATIONS(TYPE)

#define IMPLEMENT_VIRTUAL_SAVELOAD_ABILITIES(TYPE)                                                                                                                                 \
    IMPLEMENT_SAVELOAD_ABILITIES_IMPL(TYPE)                                                                                                                                        \
    IMPLEMENT_SAVELOAD_ABILITIES_POINTERS_IMPL(TYPE)                                                                                                                               \
    IMPLEMENT_SAVELOAD_ABILITIES_INSTANCIATIONS(TYPE)

#define IMPLEMENT_SAVELOAD_ABILITIES_FREEFUNC(TYPE)                                                                                                                                \
    template<>                                                                                                                                                                     \
    void SaveLoad<SavingSystem::SaveChunk, TYPE, true>(SavingSystem::SaveChunk & parChunk, TYPE & parValue)                                                                        \
    {                                                                                                                                                                              \
        u32 id = typeid(TYPE).hash_code();                                                                                                                                         \
        u32 size = sizeof(TYPE);                                                                                                                                                   \
        u8* data = reinterpret_cast<u8*>(&parValue);                                                                                                                               \
        parChunk.GetBuffer().WriteData(id, size, data);                                                                                                                            \
    }                                                                                                                                                                              \
                                                                                                                                                                                   \
    template<>                                                                                                                                                                     \
    void SaveLoad<SavingSystem::ReadChunk, TYPE, false>(SavingSystem::ReadChunk & parChunk, TYPE & parValue)                                                                       \
    {                                                                                                                                                                              \
        u8* data = reinterpret_cast<u8*>(&parValue);                                                                                                                               \
        GenericLoad(parChunk, data, typeid(TYPE).hash_code(), sizeof(TYPE));                                                                                                       \
    }
