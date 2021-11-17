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
    SELECTION_PASS,
    SELECTION_BLIT_PASS,
    FEEDBACK_PASS,
    OUTLINE_INIT,
    OUTLINE_SOLID,
    OUTLINE_HORIZONTAL,
    OUTLINE_VERTICAL,
    GAME_UI_PASS,
    COMBINE_PASS,
    EDITOR_PASS,
    EDITOR_UI_PASS,
    DEBUG_PASS = 252,
    IMGUI_EDITOR_PASS = 253,
    IMGUI_UI_PASS = 254,
    IMGUI_DEBUG_PASS = 255,
    IMGUI_PASSES_START = IMGUI_EDITOR_PASS,
    IMGUI_PASSES_END = IMGUI_DEBUG_PASS,
    END_OF_GAME_PASSES = COMBINE_PASS + 1
};
const char* GetName(Type parPass);
Type ChooseInList(RenderPassId::Type parPreviouslyChosen);
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
