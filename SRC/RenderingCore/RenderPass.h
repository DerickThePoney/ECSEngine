#pragma once

namespace ECSEngine
{
namespace Rendering
{
namespace RenderPassId
{
enum Type : u16
{
    GEOMETRY_PASS = 0,
    SELECTION_PASS = 1,
    SELECTION_BLIT_PASS = 2,
    IMGUI_DEBUG_PASS = 255
};
}
} // namespace Rendering
} // namespace ECSEngine
