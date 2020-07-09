#include "stdafx.h"

#include "EntityWorld.h"

#include "Common/Constants.h"
#include "Entity.h"
#include "EntityTemplate.h"
#include "Module.h"
#include "ModuleController.h"

namespace ECSEngine
{
EntityWorld::EntityWorld(const Worlds::Type parWorldId)
    : FSize((u32)EModuleId::Length)
    , FWorldID(parWorldId)
{
    AssertRelease(FWorldID < Worlds::LENGTH);
    AssertRelease(FSize != 0);

    FEntityIdGenerator.SetWorldId(FWorldID);

    FControllers.resize((std::size_t)EModuleId::Length);
    forrange(i, 0, FSize) { FControllers[i] = nullptr; }
    FEntities.resize(ModulePoolSize);
}

EntityWorld::~EntityWorld()
{
    AssertRelease(FSize != 0);

    DestroyAllRemainingEntities();

    for (u32 i = 0; i < FSize; ++i)
    {
        if (FControllers[i] != nullptr)
        {
            delete FControllers[i];
            FControllers[i] = nullptr;
        }
    }

    FControllers.clear();
}

EntityId EntityWorld::CreateEntityFromTemplateReturnEntityId(const EntityTemplate* parTemplate, const ModuleParameters::ParameterContainer& parParameterContainer)
{
    EntityId newID = FEntityIdGenerator.GetNextEntityId();
    AssertRelease(FAllocatedEntities.find(newID.GetSequentialId()) == FAllocatedEntities.end());

    Entity newEntity(newID, parTemplate);

    if (newID.GetSequentialId() >= FEntities.size())
        FEntities.resize(FEntities.size() + ModulePoolSize);

    FEntities[newID.GetSequentialId()] = newEntity;
    FAllocatedEntities.insert(newID.GetSequentialId());

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
    AssertRelease(FAllocatedEntities.find(parId.GetSequentialId()) != FAllocatedEntities.end());
    FEntityIdGenerator.ReleaseEntityId(parId);

    const u32 sequentialId = parId.GetSequentialId();

    const Entity& entity = FEntities[sequentialId];

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

    FAllocatedEntities.erase(sequentialId);
    FEntities[sequentialId] = Entity();
}

void EntityWorld::DestroyAllRemainingEntities()
{
    while (!FAllocatedEntities.empty())
    {
        const u32 id = *(FAllocatedEntities.begin());
        const EntityId& allocatedId = FEntities[id].GetEntityId();
        AssertRelease(allocatedId.Valid());
        DestroyEntity(allocatedId);
    }

    AssertRelease(FAllocatedEntities.empty());
#ifdef PERFORM_SECURITY_CHECKS
    foreachitemconst(entity, FEntities) { AssertRelease(!entity.GetEntityId().Valid()); }
#endif
}

const EntityTemplate* EntityWorld::GetTemplateForEntity(const EntityId& parId)
{
    AssertRelease(FAllocatedEntities.find(parId.GetSequentialId()) != FAllocatedEntities.end());
    return FEntities[parId.GetSequentialId()].GetTemplate();
}

} // namespace ECSEngine