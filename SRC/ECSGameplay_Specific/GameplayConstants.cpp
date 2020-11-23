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
} // namespace GameplayConstants

void GameplayConstantsLoader::PostSerialize()
{
    GameplayConstants::PeonFeeding::PeonEatQuantity = FPeonEatQuantity;
}

void GameplayConstantsLoader::DrawEditor()
{
    if (ImGui::CollapsingHeader("PeonFeeding"))
    {
        EDITOR_PROPERTY_SIMPLE("Peon eat quantity", FPeonEatQuantity);
    }

    PostSerialize();
}

} // namespace ECSEngine