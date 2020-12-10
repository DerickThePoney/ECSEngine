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
    FEEDBACK_PASS = 3,
    COMBINE_PASS = 4,
    EDITOR_PASS = 5,
    DEBUG_PASS = 252,
    IMGUI_EDITOR_PASS = 253,
    IMGUI_UI_PASS = 254,
    IMGUI_DEBUG_PASS = 255,
    IMGUI_PASSES_START = IMGUI_EDITOR_PASS,
    IMGUI_PASSES_END = IMGUI_DEBUG_PASS
};
const char* GetName(Type parPass);
} // namespace RenderPassId

class RenderPassDescriptor
{
public:
    RenderPassDescriptor();
    ~RenderPassDescriptor();

private:
    // pass in a frame buffer ?
    // pass in a material for the whole pass ?
    // pass in renderstate ?
    // Filter for objects ?
    // Culling parameters ?
};

} // namespace Rendering
} // namespace ECSEngine
