#include "stdafx.h"

#include "Entity.h"

#include "EntityTemplate.h"
#include "ModuleId.h"

namespace ECSEngine
{

Entity::Entity(const EntityId& parId, const EntityTemplate* parTemplate)
    : Fid(parId)
    , FTemplate(parTemplate)
{
}

Entity::~Entity()
{
}

const bool Entity::HasModule(const u32 parModuleId) const
{
    AssertRelease(FTemplate != nullptr);
    return FTemplate->HasModule(parModuleId);
}

template<typename T>
const bool Entity::HasModule() const
{
    AssertRelease(FTemplate != nullptr);
    return FTemplate->HasModule<T>();
}

#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE) template const bool Entity::HasModule<NAME>() const;
#include "ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE

} // namespace ECSEngine
