#pragma once
#include "Common/PoolAllocator.h"

namespace ECSEngine
{
namespace Rendering
{
class GFXSelectable
{
    DECLARE_POOL_ALLOCATED(GFXSelectable);

public:
    void Init(bool parSelectable);

    bool Selectable() const { return FIsSelectable; }

    void SetSelectable(bool parValue) { FIsSelectable = parValue; }

private:
    bool FIsSelectable = true;
};
} // namespace Rendering
} // namespace ECSEngine