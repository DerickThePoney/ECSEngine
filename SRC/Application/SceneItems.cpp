#include "stdafx.h"

#include "SceneItems.h"

#include "PropertyDrawer.h"
#include "SceneItemsIds.h"

namespace ECSEngine
{
IMPLEMENT_POOL_ALLOCATED(BaseSceneItem);

BaseSceneItem::BaseSceneItem()
    : FName("Scene Item")
    , FPosition(vec3(0.f))
    , FEulerAngles(vec3(0.f))
    , FId(-1)
    , FShowItem(false)
    , FItemHovered(false)
    , FItemSelected(false)
#ifdef PERFORM_SECURITY_CHECKS
    , FVirtualDrawEditorHasBeenCalled(false)
#endif
{
}

BaseSceneItem::BaseSceneItem(const std::string& parName, const u32 parId)
    : FName(parName)
    , FPosition(vec3(0.f))
    , FEulerAngles(vec3(0.f))
    , FId(parId)
    , FShowItem(false)
    , FItemHovered(false)
    , FItemSelected(false)
#ifdef PERFORM_SECURITY_CHECKS
    , FVirtualDrawEditorHasBeenCalled(false)
#endif
{
}

u32 BaseSceneItem::GetSceneItemTypeId() const
{
    return SceneItemTraits<BaseSceneItem>::GetSceneItemTypeId();
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

        vec3 eulerDegrees = Degree(FEulerAngles);
        EDITOR_PROPERTY_SIMPLE("Euler angles", eulerDegrees);
        FEulerAngles = Radians(eulerDegrees);

        ImGui::Separator();
    }
    ImGui::PopID();

#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDrawEditorHasBeenCalled = true;
#endif
    return FShowItem;
}

} // namespace ECSEngine
