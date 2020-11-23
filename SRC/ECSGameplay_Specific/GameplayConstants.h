#pragma once

namespace ECSEngine
{
namespace GameplayConstants
{
namespace PeonFeeding
{
extern u32 PeonEatQuantity;
}
} // namespace GameplayConstants

// work it out as actual extern X Y;
class GameplayConstantsLoader
{
public:
    SERIALIZE()
    {
        PROPERTYFIELD(PeonEatQuantity, 1);
        PostSerialize();
    }

    void PostSerialize();

    void DrawEditor();

private:
    u32 FPeonEatQuantity = 1;
};
} // namespace ECSEngine