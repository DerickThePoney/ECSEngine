#include "stdafx.h"

#include "EntityWorld.h"

#include "Common/Constants.h"
#include "Common/SavingSystemImplementation.h"
#include "Entity.h"
#include "EntityTemplate.h"
#include "Module.h"
#include "ModuleController.h"
#include "ModuleTemplate.h"
#include "WorldIds.h"

namespace ECSEngine
{
IMPLEMENT_SAVELOAD_ABILITIES(EntityWorld);
template<typename Chunk, bool isWriting>
void EntityWorld::SaveLoad(Chunk& parChunk)
{
    if (!isWriting)
    {
        // Cleanup !!
        DestroyAllRemainingEntities();
    }

    parChunk & FSize;
    u32 worldId = (u32)FWorldID;
    parChunk & worldId;
    if (!isWriting)
        FWorldID = (EEntityWorlds)worldId;
    parChunk & FEntityIdGenerator;
    parChunk & FAllocatedEntities;

    u32 entitiesSize = FEntities.size();
    parChunk & entitiesSize;

    if (!isWriting)
    {
        FEntities.clear();
        FEntities.resize(entitiesSize);
    }

    foreachitemconst(entity, FAllocatedEntities) parChunk& FEntities[entity];

    u32 nonNullControllers = 0;
    if (isWriting)
    {
        foreachitemconst(ctr, FControllers)
        {
            if (ctr == nullptr)
                continue;
            nonNullControllers++;
        }
    }

    parChunk & nonNullControllers;
    if (isWriting)
    {
        forrange(i, 0, FControllers.size())
        {
            if (FControllers[i] == nullptr)
                continue;
            parChunk & i;
            parChunk& FControllers[i];
        }
    }
    else
    {
        forrange(i, 0, nonNullControllers)
        {
            size_t idx = -1;
            parChunk & idx;
            AssertRelease(idx != -1);
            AssertRelease(idx < FControllers.size());
            parChunk& FControllers[idx];
        }
    }
}

EntityWorld::EntityWorld(const EEntityWorlds parWorldId)
    : FSize((u32)EModuleId::Length)
    , FWorldID(parWorldId)
{
    AssertRelease(FWorldID < EEntityWorlds::LENGTH);
    AssertRelease(FSize != 0);

    FEntityIdGenerator.SetWorldId(FWorldID);

    FControllers.resize((std::size_t)EModuleId::Length);
    forrange(i, 0, FSize)
    {
        FControllers[i] = nullptr;
    }
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

EntityId EntityWorld::CreateNewEntityId()
{
    return FEntityIdGenerator.GetNextEntityId();
}

void EntityWorld::CreateEntityFromTemplateUsingEntityId(const EntityId& parUnitId,
      const EntityTemplate* parTemplate,
      const ModuleParameters::ParameterContainer& parParameterContainer)
{
    AssertRelease(FAllocatedEntities.find(parUnitId.GetSequentialId()) == FAllocatedEntities.end());

    Entity newEntity(parUnitId, parTemplate);

    if (parUnitId.GetSequentialId() >= FEntities.size())
        FEntities.resize(FEntities.size() + ModulePoolSize);

    FEntities[parUnitId.GetSequentialId()] = newEntity;
    FAllocatedEntities.insert(parUnitId.GetSequentialId());

    std::vector<Module*> createdModules;

    for (u32 i = 0; i < (u32)EModuleId::Length; ++i)
    {
        if (parTemplate->HasModule(i))
        {
            const ModuleTemplate* modTemp = parTemplate->GetModuleTemplate(i);
            AssertRelease(modTemp != nullptr);
            Module* newMod = modTemp->CreateInstance(parUnitId, parParameterContainer);
            createdModules.push_back(newMod);
        }
    }

    for (Module* mod : createdModules)
    {
        mod->PostInit();
    }
}

void EntityWorld::DestroyEntity(const EntityId& parId)
{
    AssertRelease(parId.Valid());
    AssertRelease(parId.GetWorld() == FWorldID);
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
    foreachitemconst(entity, FEntities)
    {
        AssertRelease(!entity.GetEntityId().Valid());
    }
#endif
}

const EntityTemplate* EntityWorld::GetTemplateForEntity(const EntityId& parId) const
{
    AssertRelease(FAllocatedEntities.find(parId.GetSequentialId()) != FAllocatedEntities.end());
    return FEntities[parId.GetSequentialId()].GetTemplate();
}

void EntityWorld::OnLoaded()
{
    foreachitem(ctr, FControllers)
    {
        if (ctr == nullptr)
            continue;
        ctr->OnLoaded();
    }
}

} // namespace ECSEngine
