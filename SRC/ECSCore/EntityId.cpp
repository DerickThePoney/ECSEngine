#include "stdafx.h"

#include "EntityId.h"
namespace ECSEngine
{
EntityId::EntityId(u32 parWorldId, u32 parId)
    : FId(parWorldId, parId)
{
}
} // namespace ECSEngine