#include "stdafx.h"

#include "SaveLoadData.h"

#include "ECSCore/EntityId.h"

#include <string>

namespace ECSEngine
{
/**********************************************************************************/
/*                          SAVING FUNCTIONS                                      */
/**********************************************************************************/
template<>
void SaveLoad<SavingSystem::SaveChunk, std::size_t, true>(SavingSystem::SaveChunk& parChunk, std::size_t& parValue)
{
    u32 id = typeid(std::size_t).hash_code();
    u32 size = sizeof(std::size_t);
    u8* data = reinterpret_cast<u8*>(&parValue);
    parChunk.GetBuffer().WriteData(id, size, data);
}

template<>
void SaveLoad<SavingSystem::SaveChunk, bool, true>(SavingSystem::SaveChunk& parChunk, bool& parValue)
{
    u32 id = typeid(bool).hash_code();
    u32 size = sizeof(bool);
    u8* data = reinterpret_cast<u8*>(&parValue);
    parChunk.GetBuffer().WriteData(id, size, data);
}

template<>
void SaveLoad<SavingSystem::SaveChunk, i32, true>(SavingSystem::SaveChunk& parChunk, i32& parValue)
{
    u32 id = typeid(i32).hash_code();
    u32 size = sizeof(i32);
    u8* data = reinterpret_cast<u8*>(&parValue);
    parChunk.GetBuffer().WriteData(id, size, data);
}

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

template<>
void SaveLoad<SavingSystem::SaveChunk, glm::quat, true>(SavingSystem::SaveChunk& parChunk, glm::quat& parValue)
{
    u32 id = typeid(glm::quat).hash_code();
    u32 size = sizeof(glm::quat);
    u8* data = reinterpret_cast<u8*>(&parValue);
    parChunk.GetBuffer().WriteData(id, size, data);
}

template<>
void SaveLoad<SavingSystem::SaveChunk, EntityId, true>(SavingSystem::SaveChunk& parChunk, EntityId& parValue)
{
    u32 id = typeid(EntityId).hash_code();
    u32 size = sizeof(EntityId);
    u8* data = reinterpret_cast<u8*>(&parValue);
    parChunk.GetBuffer().WriteData(id, size, data);
}

template<>
void SaveLoad<SavingSystem::SaveChunk, std::string, true>(SavingSystem::SaveChunk& parChunk, std::string& parValue)
{
    const u32 id = typeid(std::string).hash_code();
    parChunk.GetBuffer().WriteGuards(id);

    const u32 size = parValue.size();

    u8* data = reinterpret_cast<u8*>(parValue.data());
    parChunk.GetBuffer().WriteRawData(size, data);
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
void SaveLoad<SavingSystem::ReadChunk, std::size_t, false>(SavingSystem::ReadChunk& parChunk, std::size_t& parValue)
{
    u8* data = reinterpret_cast<u8*>(&parValue);
    GenericLoad(parChunk, data, typeid(std::size_t).hash_code(), sizeof(std::size_t));
}

template<>
void SaveLoad<SavingSystem::ReadChunk, bool, false>(SavingSystem::ReadChunk& parChunk, bool& parValue)
{
    u8* data = reinterpret_cast<u8*>(&parValue);
    GenericLoad(parChunk, data, typeid(bool).hash_code(), sizeof(bool));
}

template<>
void SaveLoad<SavingSystem::ReadChunk, i32, false>(SavingSystem::ReadChunk& parChunk, i32& parValue)
{
    u8* data = reinterpret_cast<u8*>(&parValue);
    GenericLoad(parChunk, data, typeid(i32).hash_code(), sizeof(i32));
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

template<>
void SaveLoad<SavingSystem::ReadChunk, glm::quat, false>(SavingSystem::ReadChunk& parChunk, glm::quat& parValue)
{
    u8* data = reinterpret_cast<u8*>(&parValue);
    GenericLoad(parChunk, data, typeid(glm::quat).hash_code(), sizeof(glm::quat));
}

template<>
void SaveLoad<SavingSystem::ReadChunk, EntityId, false>(SavingSystem::ReadChunk& parChunk, EntityId& parValue)
{
    u8* data = reinterpret_cast<u8*>(&parValue);
    GenericLoad(parChunk, data, typeid(EntityId).hash_code(), sizeof(EntityId));
}

template<>
void SaveLoad<SavingSystem::ReadChunk, std::string, false>(SavingSystem::ReadChunk& parChunk, std::string& parValue)
{
    const u32 expectedId = typeid(std::string).hash_code();
    const u32 id = parChunk.GetBuffer().ReadId();
    AlwaysCheckedAssert(id == expectedId);

    u32 stringSize = parChunk.GetBuffer().ReadSize();
    parValue.resize(stringSize);
    u8* data = reinterpret_cast<u8*>(parValue.data());
    parChunk.GetBuffer().ReadData(stringSize, data);
}

namespace SavingSystem
{

void SaveChunk::WriteIdAndSize(u32 id, u32 size)
{
    FDataBuffer.WriteIdAndSize(id, size);
}

void ReadChunk::ReadIdAndSize(u32& id, u32& size)
{
    id = FDataBuffer.ReadId();
    size = FDataBuffer.ReadSize();
}

} // namespace SavingSystem
} // namespace ECSEngine
