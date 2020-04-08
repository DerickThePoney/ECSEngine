#include "stdafx.h"

#include "EntityWorld.h"

#include "Common/Constants.h"
#include "Entity.h"
#include "EntityTemplate.h"
#include "Module.h"
#include "ModuleController.h"

namespace ECSEngine
{

u8 EntityWorld::sWorldIdGenerator = 0;

EntityWorld::EntityWorld()
    : FSize((u32)EModuleId::Length)
    , FControllers(nullptr)
    , FWorldID(sWorldIdGenerator++)
    , FEntityIdGenerator()
{
    AssertRelease(FSize != 0);

    FControllers = new IModuleController*[FSize];
    forrange(i, 0, FSize) FControllers[i] = nullptr;
    FEntities.resize(ModulePoolSize);
    FEntityIdGenerator.SetWorldId(FWorldID);
}

EntityWorld::EntityWorld(EntityWorld&& other) noexcept
{
    FSize = other.FSize;
    FControllers = other.FControllers;
    other.FControllers = nullptr;
    FEntities = std::move(other.FEntities);
    other.FEntities.clear();

    FWorldID = other.FWorldID;
    FEntityIdGenerator = std::move(other.FEntityIdGenerator);
}

void EntityWorld::operator=(EntityWorld&& other) noexcept
{
    FSize = other.FSize;
    FControllers = other.FControllers;
    other.FControllers = nullptr;
    FEntities = std::move(other.FEntities);
    other.FEntities.clear();

    FWorldID = other.FWorldID;
    FEntityIdGenerator = std::move(other.FEntityIdGenerator);
}

EntityWorld::~EntityWorld()
{
    AssertRelease(FSize != 0);

    if (FControllers != nullptr)
    {

        for (u32 i = 0; i < FSize; ++i)
        {
            if (FControllers[i] != nullptr)
            {
                delete FControllers[i];
                FControllers[i] = nullptr;
            }
        }

        delete[] FControllers;
        FControllers = nullptr;
    }
}

EntityId EntityWorld::CreateEntityFromTemplateReturnEntityId(const EntityTemplate* parTemplate, const ModuleParameters::ParameterContainer& parParameterContainer)
{
    EntityId newID = FEntityIdGenerator.GetNextEntityId();
    FAllocatedEntityIds.insert(newID);

    Entity newEntity(newID, parTemplate);

    if (newID.GetSequentialId() >= FEntities.size())
        FEntities.resize(FEntities.size() + ModulePoolSize);

    FEntities[newID.GetSequentialId()] = newEntity;

    for (u32 i = 0; i < (u32)EModuleId::Length; ++i)
    {
        if (parTemplate->HasModule(i))
        {
            const ModuleTemplate* modTemp = parTemplate->GetModuleTemplate(i);
            AssertRelease(modTemp != nullptr);
            modTemp->CreateInstance(newID, parParameterContainer);
        }
    }

    return newID;
}

void EntityWorld::DestroyEntity(const EntityId& parId)
{
    AssertRelease(parId.Valid());
    AssertRelease(parId.GetWorldId() == FWorldID);
    AssertRelease(FAllocatedEntityIds.find(parId) != FAllocatedEntityIds.end());
    FAllocatedEntityIds.erase(parId);
    FEntityIdGenerator.ReleaseEntityId(parId);

    const Entity& entity = FEntities[parId.GetSequentialId()];

    for (u32 i = 0; i < (u32)EModuleId::Length; ++i)
    {
        if (entity.HasModule(i))
        {
            FControllers[i]->GetModulePtrForEntity(parId)->Deinit();
        }
    }

    for (u32 i = 0; i < (u32)EModuleId::Length; ++i)
    {
        if (entity.HasModule(i))
        {
            FControllers[i]->DeallocateForEntity(parId);
        }
    }

    AssertReleaseMsg(entity.GetEntityId().GetReferenceCounter()->GetRefCounts() == 1, "An EntityId object still references this id, not good, not good at all");

    FEntities[parId.GetSequentialId()] = Entity();
}

const EntityTemplate* EntityWorld::GetTemplateForEntity(const EntityId& parId)
{
    AssertRelease(FAllocatedEntityIds.find(parId) != FAllocatedEntityIds.end());

    return FEntities[parId.GetSequentialId()].GetTemplate();
}

} // namespace ECSEngine