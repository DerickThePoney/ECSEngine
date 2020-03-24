#include "stdafx.h"

#include "SceneItems.h"

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
        ImGui::InputFloat3("Position", (float*)&FPosition);
        ImGui::InputFloat4("Orientation", (float*)&FOrientation);
        ImGui::Separator();
    }

    FVirtualDrawEditorHasBeenCalled = true;
    return FShowItem;
}

} // namespace ECSEngine
