#include "stdafx.h"

#include "GameplayConstants.h"

#include "Application/PropertyDrawer.h"

namespace ECSEngine
{
namespace GameplayConstants
{
namespace PeonFeeding
{
u32 PeonEatQuantity = 1;
}
namespace Colony
{
float ColonyInitialRange = 20.f;
float ColonyRangeFeedbackThickness = 1.f;
glm::vec4 ColonyRangeFeedbackColor;
} // namespace Colony
} // namespace GameplayConstants

void GameplayConstantsLoader::PostSerialize()
{
    // Peon feeding
    GameplayConstants::PeonFeeding::PeonEatQuantity = FPeonEatQuantity;

    // Colony
    GameplayConstants::Colony::ColonyInitialRange = FColonyInitialRange;
    GameplayConstants::Colony::ColonyRangeFeedbackThickness = FColonyRangeFeedbackThickness;
    GameplayConstants::Colony::ColonyRangeFeedbackColor = FColonyRangeFeedbackColor;
}

void GameplayConstantsLoader::DrawEditor()
{
    if (ImGui::CollapsingHeader("PeonFeeding"))
    {
        EDITOR_PROPERTY_SIMPLE("Peon eat quantity", FPeonEatQuantity);
    }

    if (ImGui::CollapsingHeader("Colony"))
    {
        EDITOR_PROPERTY_SIMPLE("Colony initial range", FColonyInitialRange);
        EDITOR_PROPERTY_SIMPLE("Colony range feedback thickness", FColonyRangeFeedbackThickness);
        EDITOR_PROPERTY_COLOR("Colony range feedback color", FColonyRangeFeedbackColor);
    }

    PostSerialize();
}

} // namespace ECSEngine