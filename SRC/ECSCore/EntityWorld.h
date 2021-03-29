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
    EntityWorld(const Worlds::Type parWorldId);
    EntityWorld(EntityWorld&& other) noexcept = delete;
    void operator=(EntityWorld&& other) noexcept = delete;
    ~EntityWorld();

    template<typename T>
    void AddController();

    template<typename T>
    IModuleController* GetControllerIFP();

    EntityId CreateEntityFromTemplateReturnEntityId(const EntityTemplate* parTemplate, const ModuleParameters::ParameterContainer& parParameterContainer);
    void DestroyEntity(const EntityId& parId);
    void DestroyAllRemainingEntities();

    const u32 WorldID() const { return FWorldID; }

    const EntityTemplate* GetTemplateForEntity(const EntityId& parId);

private:
    u32 FSize;
    u32 FWorldID;

    EntityIDGenerator FEntityIdGenerator;
    std::vector<Entity> FEntities;
    std::set<u32> FAllocatedEntities;

    std::vector<IModuleController*> FControllers;
};

} // namespace ECSEngine

#include "EntityWorld.inl"
