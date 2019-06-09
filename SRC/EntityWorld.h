#pragma once
#include "Entity.h"

namespace ECSEngine
{
class IModuleController;
class EntityId;
class EntityWorld
{
public:
    EntityWorld();
    EntityWorld(EntityWorld&& other) noexcept;
    ~EntityWorld();

    template<typename T>
    void AddController();

    template<typename T>
    IModuleController* GetControllerIFP();

    EntityId CreateEntityFromTemplateReturnEntityId(EntityTemplate* parTemplate);
    void DestroyEntity(const EntityId& parId);

    const u8 WorldID() const { return FWorldID; }

private:
    IModuleController** FControllers;
    std::vector<Entity> FEntities;
    std::set<EntityId> FAllocatedEntityIds;
    u32 FSize;
    u32 FEntityIdGenerator;
    u8 FWorldID;

    static u8 sWorldIdGenerator;
};

} // namespace ECSEngine

#include "EntityWorld.inl"