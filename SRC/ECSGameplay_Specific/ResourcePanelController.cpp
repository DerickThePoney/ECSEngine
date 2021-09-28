#include "stdafx.h"

#include "ResourcePanelController.h"

#include "UICore/RMLUIManager.h"
#include "UICore/RML_includes.h"

namespace ECSEngine
{
namespace UI
{

ResourcePanelController::ResourcePanelController()
{
}

void ResourcePanelController::VirtualInit()
{
    UIController::VirtualInit();

    FDocument = RmlUiManager::Instance().LoadDocument("UI\\ResourcesPanel\\ResourcesPanel.rml");
    AssertRelease(FDocument != nullptr);
    FDocument->GetElementById("title")->SetInnerRML(FDocument->GetTitle());
}

void ResourcePanelController::VirtualUpdate()
{
    UIController::VirtualUpdate();

    if (!HandleVisibility())
        return;
}

void ResourcePanelController::VirtualDestroy()
{
    UIController::VirtualDestroy();

    if (FDocument != nullptr)
    {
        RmlUiManager::Instance().UnloadDocument(FDocument);
    }
}

bool ResourcePanelController::HandleVisibility()
{
    const bool visible = FDocument->IsVisible();
    if (FShow && !visible)
    {
        FDocument->Show();
    }
    else if (!FShow && visible)
    {
        FDocument->Hide();
    }
    return FShow;
}

} // namespace UI
} // namespace ECSEngine
