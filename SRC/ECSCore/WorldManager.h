#pragma once
#include "Common/SavingSystemDeclaration.h"
#include "Common/Singleton.h"
#include "WorldIds_fwd.h"
#include "ModuleParameters.h"

namespace ECSEngine
{
class EntityWorld;
class EntityTemplate;
class EntityId;

using UnitDeathListener = Delegate<void(const EntityId)>;

class WorldManager final : public Singleton<WorldManager>
{
    DECLARE_SAVELOAD_ABILITIES();

public:
    WorldManager();
    ~WorldManager();

    void Init();
    void Shutdown();

    void AddEntityWorldStealOwnership(EEntityWorlds parType, EntityWorld* parWorld);
    void ProcessDestroyEntities();
    void DestroyAllRemainingEntities();
    void ProcessWithEntitiesCreation();

    EntityWorld& GetWorld(EEntityWorlds parWorld);
    const EntityWorld& GetWorld(EEntityWorlds parWorld) const;
    EntityWorld* GetWorldIFP(EEntityWorlds parWorld);
    const EntityWorld* GetWorldIFP(EEntityWorlds parWorld) const;

    void MarkAsDead(const EntityId& parId);
    void RequestCreateEntity(const EntityId parUnitId, const EntityTemplate* parTemplate, const ModuleParameters::ParameterContainer& parParameterContainer);

    const EntityTemplate* GetTemplateForEntityId(const EntityId& parUnitId) const;

    void RegisterDeathListener(UnitDeathListener parListener);
    void RemoveDeathListener(UnitDeathListener parListener);

private:
    std::vector<std::unique_ptr<EntityWorld>> FWorlds;
    std::set<EntityId> FDeadEntities;

    struct EntityCreationMessage
    {
        const EntityId FUnitId;
        const EntityTemplate* FTemplate;
        ModuleParameters::ParameterContainer FParameterContainer;
    };
    std::vector<EntityCreationMessage> FEntitiesToCreate;


    std::vector<UnitDeathListener> FUnitDeathListeners;
};
} // namespace ECSEngine
