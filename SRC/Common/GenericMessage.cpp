#include "stdafx.h"

#include "GenericMessage.h"

namespace ECSEngine
{
GenericMessage::GenericMessage(u32 parId, void* parMessageDataToStealOwnershipOf)
    : FId(parId)
    , FUserData(parMessageDataToStealOwnershipOf)
{
}

} // namespace ECSEngine
