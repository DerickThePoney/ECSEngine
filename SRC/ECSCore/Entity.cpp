#include "stdafx.h"

#include "Entity.h"

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
} // namespace ECSEngine
