#pragma once

namespace ECSEngine
{
namespace GameplayConstants
{
namespace PeonFeeding
{
extern u32 PeonEatQuantity;
}
namespace Colony
{
extern float ColonyInitialRange;
extern float ColonyRangeFeedbackThickness;
extern glm::vec4 ColonyRangeFeedbackColor;
} // namespace Colony
} // namespace GameplayConstants

// work it out as actual extern X Y;
class GameplayConstantsLoader
{
public:
    SERIALIZE()
    {
        PROPERTYFIELD(PeonEatQuantity, 1);

        PROPERTYFIELD(ColonyInitialRange, 20.f);
        PROPERTYFIELD(ColonyRangeFeedbackThickness, 1.f);
        PROPERTYFIELD(ColonyRangeFeedbackColor, glm::vec4(0.f));

        PostSerialize();
    }

    void PostSerialize();

    void DrawEditor();

private:
    u32 FPeonEatQuantity = 1;

    float FColonyInitialRange = 20.f;
    float FColonyRangeFeedbackThickness = 1.f;
    glm::vec4 FColonyRangeFeedbackColor = glm::vec4(0.f);
};
} // namespace ECSEngine