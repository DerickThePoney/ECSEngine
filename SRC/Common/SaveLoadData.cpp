#include "stdafx.h"

#include "SaveLoadData.h"

namespace ECSEngine
{
/**********************************************************************************/
/*                          SAVING FUNCTIONS                                      */
/**********************************************************************************/
template<>
void SaveLoad<SavingSystem::SaveChunk, u32, true>(SavingSystem::SaveChunk& parChunk, u32& parValue)
{
    u32 id = typeid(u32).hash_code();
    u32 size = sizeof(u32);
    u8* data = reinterpret_cast<u8*>(&parValue);
    parChunk.GetBuffer().WriteData(id, size, data);
}

template<>
void SaveLoad<SavingSystem::SaveChunk, float, true>(SavingSystem::SaveChunk& parChunk, float& parValue)
{
    u32 id = typeid(float).hash_code();
    u32 size = sizeof(float);
    u8* data = reinterpret_cast<u8*>(&parValue);
    parChunk.GetBuffer().WriteData(id, size, data);
}

template<>
void SaveLoad<SavingSystem::SaveChunk, glm::vec3, true>(SavingSystem::SaveChunk& parChunk, glm::vec3& parValue)
{
    u32 id = typeid(glm::vec3).hash_code();
    u32 size = sizeof(glm::vec3);
    u8* data = reinterpret_cast<u8*>(&parValue);
    parChunk.GetBuffer().WriteData(id, size, data);
}

/**********************************************************************************/
/*                          LOADING FUNCTIONS                                     */
/**********************************************************************************/

void GenericLoad(SavingSystem::ReadChunk& parChunk, u8* parData, u32 expectedId, u32 expectedSize)
{
    u32 id = parChunk.GetBuffer().ReadId();
    u32 size = parChunk.GetBuffer().ReadSize();
    AlwaysCheckedAssert(id == expectedId);
    AlwaysCheckedAssert(size == expectedSize);

    parChunk.GetBuffer().ReadData(size, parData);
}

template<>
void SaveLoad<SavingSystem::ReadChunk, u32, false>(SavingSystem::ReadChunk& parChunk, u32& parValue)
{
    u8* data = reinterpret_cast<u8*>(&parValue);
    GenericLoad(parChunk, data, typeid(u32).hash_code(), sizeof(u32));
}

template<>
void SaveLoad<SavingSystem::ReadChunk, float, false>(SavingSystem::ReadChunk& parChunk, float& parValue)
{
    u8* data = reinterpret_cast<u8*>(&parValue);
    GenericLoad(parChunk, data, typeid(float).hash_code(), sizeof(float));
}

template<>
void SaveLoad<SavingSystem::ReadChunk, glm::vec3, false>(SavingSystem::ReadChunk& parChunk, glm::vec3& parValue)
{
    u8* data = reinterpret_cast<u8*>(&parValue);
    GenericLoad(parChunk, data, typeid(glm::vec3).hash_code(), sizeof(glm::vec3));
}

namespace SavingSystem
{
} // namespace SavingSystem
} // namespace ECSEngine
