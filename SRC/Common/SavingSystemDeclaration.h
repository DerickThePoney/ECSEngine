#pragma once

namespace ECSEngine
{
namespace SavingSystem
{
class SaveChunk;
class ReadChunk;
} // namespace SavingSystem
} // namespace ECSEngine

#define DECLARE_SAVELOAD_ABILITIES()                                                                                                                                               \
public:                                                                                                                                                                            \
    void SaveLoad(SavingSystem::SaveChunk&);                                                                                                                                       \
    void SaveLoad(SavingSystem::ReadChunk&);                                                                                                                                       \
    template<typename Chunk, bool isWriting>                                                                                                                                       \
    void SaveLoad(Chunk&);                                                                                                                                                         \
                                                                                                                                                                                   \
private:

#define DECLARE_VIRTUAL_SAVELOAD_ABILITIES()                                                                                                                                       \
public:                                                                                                                                                                            \
    virtual void SaveLoad(SavingSystem::SaveChunk&);                                                                                                                               \
    virtual void SaveLoad(SavingSystem::ReadChunk&);                                                                                                                               \
    template<typename Chunk, bool isWriting>                                                                                                                                       \
    void SaveLoad(Chunk&);                                                                                                                                                         \
                                                                                                                                                                                   \
private: