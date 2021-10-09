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

    bool IsSelected() const { return FIsSelected; }
    void SetSelected(bool parValue) { FIsSelected = parValue; }

    bool IsHighlighted() const { return FIsHighlighted; }
    void SetHighlighted(bool parValue) { FIsHighlighted = parValue; }

private:
    bool FIsSelectable = true;
    bool FIsSelected = false;
    bool FIsHighlighted = false;
};
} // namespace Rendering
} // namespace ECSEngine