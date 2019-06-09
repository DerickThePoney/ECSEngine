#include "stdafx.h"

#include "EntityId.h"
namespace ECSEngine
{
EntityId::EntityId(u8 parWorldId, u32 parId)
    : FId(parWorldId, parId)
{
}
} // namespace ECSEngine