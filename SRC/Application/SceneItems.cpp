#include "stdafx.h"

#include "SceneItems.h"

#include "PropertyDrawer.h"

namespace ECSEngine
{

BaseSceneItem::BaseSceneItem()
    : FName("Scene Item")
    , FPosition(glm::vec3(0.f))
    , FOrientation(glm::quat())
    , FShowItem(false)
#ifdef PERFORM_SECURITY_CHECKS
    , FVirtualDrawEditorHasBeenCalled(false)
#endif
{
}

BaseSceneItem::BaseSceneItem(const std::string& parName)
    : FName("Scene Item")
    , FPosition(glm::vec3(0.f))
    , FOrientation(glm::quat())
    , FShowItem(false)
#ifdef PERFORM_SECURITY_CHECKS
    , FVirtualDrawEditorHasBeenCalled(false)
#endif
{
}

void BaseSceneItem::DrawEditor()
{

#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDrawEditorHasBeenCalled = false;
#endif
    VirtualDrawEditor();
#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssertMsg(FVirtualDrawEditorHasBeenCalled, "Un appel virtuel à VirtualDrawEditor a été manqué");
#endif
}

bool BaseSceneItem::VirtualDrawEditor()
{
    ImGui::PushID(ImGui::GetID(this));
    ImGui::CollapsingHeader(FName.c_str(), &FShowItem);
    ImGui::PopID();

    if (FShowItem)
    {
        EDITOR_PROPERTY_STRING("Scene item name", FName, false, "");

        EDITOR_PROPERTY_SIMPLE("Position", FPosition);
        EDITOR_PROPERTY_SIMPLE("Orientation", FOrientation);
        ImGui::Separator();
    }

    FVirtualDrawEditorHasBeenCalled = true;
    return FShowItem;
}

} // namespace ECSEngine
