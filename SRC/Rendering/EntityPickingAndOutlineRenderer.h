#pragma once

namespace ECSEngine
{
namespace Rendering
{
constexpr u32 PickTextureSize = 8;
class EntityPickingAndOutlineRenderer
{
public:
    // push entity for selection pass

    // submit selection pass stuffs

    // render outline IFN

private:
    u8 FSelectionData[PickTextureSize * PickTextureSize * 4];

    float FSelectionFoV = 1.f;
    u32 FSelectedEntity = -1;
    u32 FSelectedEntityHits = 0;

    bool FReadingData = false;
    bool FReadingAvailable = false;
};
} // namespace Rendering
} // namespace ECSEngine