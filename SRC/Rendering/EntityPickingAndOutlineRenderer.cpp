#include "stdafx.h"

#include "EntityPickingAndOutlineRenderer.h"

namespace ECSEngine
{
namespace Rendering
{
// framebuffer for picking

// texture for readback of selection

// Outline rendering: frame size / 4 - First pass, drawing - second pass horizontal filter - third pass vertical filter (use stencil?) - then upcale to frame resolution via
// multiple blitting - blit onto the final frame in the combine pass

} // namespace Rendering
} // namespace ECSEngine