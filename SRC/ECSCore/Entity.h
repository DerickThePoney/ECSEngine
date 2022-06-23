#pragma once
#include "Common/SavingSystemDeclaration.h"
#include "EntityId.h"

namespace ECSEngine
{
class EntityTemplate;
class Entity
{
    DECLARE_SAVELOAD_ABILITIES();

public:
    Entity(const EntityId& parId = EntityId(), const EntityTemplate* parTemplate = nullptr);
    ~Entity();

    const EntityId& GetEntityId() const { return Fid; }

    template<typename T>
    const bool HasModule() const;

    const bool HasModule(const u32 parModuleId) const;

    const EntityTemplate* GetTemplate() const { return FTemplate; }

private:
    EntityId Fid;
    const EntityTemplate* FTemplate;
};
} // namespace ECSEngine
