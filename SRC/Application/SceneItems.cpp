#include "stdafx.h"

#include "SceneItems.h"

#include "PropertyDrawer.h"

namespace ECSEngine
{
IMPLEMENT_POOL_ALLOCATED(BaseSceneItem);

BaseSceneItem::BaseSceneItem()
    : FName("Scene Item")
    , FPosition(glm::vec3(0.f))
    , FEulerAngles(glm::vec3(0.f))
    , FId(-1)
    , FShowItem(false)
#ifdef PERFORM_SECURITY_CHECKS
    , FVirtualDrawEditorHasBeenCalled(false)
#endif
{
}

BaseSceneItem::BaseSceneItem(const std::string& parName, const u32 parId)
    : FName(parName)
    , FPosition(glm::vec3(0.f))
    , FEulerAngles(glm::vec3(0.f))
    , FId(parId)
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
    FShowItem = ImGui::CollapsingHeader("", ImGuiTreeNodeFlags_CollapsingHeader);
    ImGui::SameLine();
    ImGui::Text("%s", FName.c_str());
    if (FShowItem)
    {
        EDITOR_PROPERTY_STRING("Scene item name", FName, false, "");

        EDITOR_PROPERTY_SIMPLE("Position", FPosition);

        glm::vec3 eulerDegrees = glm::degrees(FEulerAngles);
        EDITOR_PROPERTY_SIMPLE("Euler angles", eulerDegrees);
        FEulerAngles = glm::radians(eulerDegrees);

        ImGui::Separator();
    }
    ImGui::PopID();

#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDrawEditorHasBeenCalled = true;
#endif
    return FShowItem;
}

} // namespace ECSEngine
