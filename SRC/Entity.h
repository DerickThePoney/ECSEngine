#pragma once
#include "EntityId.h"
#include "EntityTemplate.h"

namespace ECSEngine
{
class EntityTemplate;
class Entity
{
public:
    Entity(const EntityId& parId = EntityId(-1, -1), const EntityTemplate* parTemplate = nullptr);
    ~Entity();

    const EntityId& GetEntityId() const { return Fid; }

    template<typename T>
    const bool HasModule() const
    {
        AssertRelease(FTemplate != nullptr);
        return FTemplate->HasModule();
    }

    const bool HasModule(const u32 parModuleId) const
    {
        AssertRelease(FTemplate != nullptr);
        return FTemplate->HasModule(parModuleId);
    }

private:
    EntityId Fid;
    const EntityTemplate* FTemplate;
};
} // namespace ECSEngine