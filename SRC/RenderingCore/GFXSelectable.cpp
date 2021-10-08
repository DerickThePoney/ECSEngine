#include "stdafx.h"

#include "GFXSelectable.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(GFXSelectable);

void GFXSelectable::Init(bool parSelectable)
{
    FIsSelectable = parSelectable;
}

} // namespace Rendering
} // namespace ECSEngine