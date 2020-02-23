#pragma once
#include "Entity.h"
#include "EntityIDGenerator.h"

namespace ECSEngine
{
namespace ModuleParameters
{
class ParameterContainer;
}
} // namespace ECSEngine

namespace ECSEngine
{
class IModuleController;
class EntityId;
class EntityWorld
{
public:
    EntityWorld();
    EntityWorld(EntityWorld&& other) noexcept;
    void operator=(EntityWorld&& other) noexcept;
    ~EntityWorld();

    template<typename T>
    void AddController();

    template<typename T>
    IModuleController* GetControllerIFP();

    EntityId CreateEntityFromTemplateReturnEntityId(const EntityTemplate* parTemplate, const ModuleParameters::ParameterContainer& parParameterContainer);
    void DestroyEntity(const EntityId& parId);

    const u8 WorldID() const { return FWorldID; }

    const EntityTemplate* GetTemplateForEntity(const EntityId& parId);

private:
    IModuleController** FControllers;
    std::vector<Entity> FEntities;
    std::set<EntityId> FAllocatedEntityIds;
    EntityIDGenerator FEntityIdGenerator;
    u32 FSize;
    u8 FWorldID;

    static u8 sWorldIdGenerator;
};

} // namespace ECSEngine

#include "EntityWorld.inl"