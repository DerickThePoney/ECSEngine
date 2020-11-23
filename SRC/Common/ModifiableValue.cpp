#include "stdafx.h"

#include "ModifiableValue.h"

namespace ECSEngine
{
namespace ModifierType
{

const char* GetName(const Type parModifierType)
{
    switch (parModifierType)
    {
    case FLAT:
        return "FLAT";
    case PERCENT:
        return "PERCENTAGE";
    default:
        AssertNotReached();
        return "UNKNOWN MODIFIER";
        break;
    }
}

} // namespace ModifierType

} // namespace ECSEngine