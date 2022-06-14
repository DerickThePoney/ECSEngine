#include "stdafx.h"

#include "ResourcePanelController.h"

#include "ResourceManager.h"
#include "UICore/RMLUIManager.h"
#include "UICore/RML_includes.h"
#include "UICore/RmlDataModelWrapper.h"

namespace ECSEngine
{
namespace UI
{

ResourcePanelController::ResourcePanelController()
{
}

ResourcePanelController::~ResourcePanelController()
{
}

void ResourcePanelController::VirtualInit()
{
    UIController::VirtualInit();

    Rml::DataModelConstructor ctr2 = RmlUiManager::Instance().CreateDataModel("resourcesModel");

    if (auto handle = ctr2.RegisterStruct<UIResourceView>())
    {
        handle.RegisterMember("name", &UIResourceView::ResourceName);
        handle.RegisterMember("quantity", &UIResourceView::Quantity);
    }

    ctr2.RegisterArray<std::vector<UIResourceView>>();

    ctr2.Bind("resources", &FResourceView);

    FDataModelWrapper = RmlDataModelWrapperFactory::CreateDataModelWrapper(ctr2.GetModelHandle());

    FDocument = RmlUiManager::Instance().LoadDocument("UI\\ResourcesPanel\\ResourcesPanel.rml");
    AssertRelease(FDocument != nullptr);
    FDocument->GetElementById("title")->SetInnerRML(FDocument->GetTitle());
}

void ResourcePanelController::VirtualUpdate()
{
    UIController::VirtualUpdate();

    if (!HandleVisibility())
        return;

    // update des resources dans la vue
    forrange(i, 0, GameResource::LENGTH)
    {
        const GameResource::Type currentResource = (GameResource::Type)i;
        const u32 currentResourceQuantity = ResourceManager::Instance().GetResourceQuantity(currentResource);

        auto itFind = FResourceToIndex.find(currentResource);
        if (itFind != FResourceToIndex.end())
        {
            AlwaysCheckedAssert(FResourceView[itFind->second].Resource == currentResource);
            FResourceView[itFind->second].Quantity = currentResourceQuantity;
        }
        else if (currentResourceQuantity > 0)
        {
            UIResourceView newView;
            newView.Resource = currentResource;
            newView.ResourceName = GameResource::GetName(currentResource);
            newView.Quantity = currentResourceQuantity;

            FResourceToIndex.insert_or_assign(currentResource, (u32)FResourceView.size());
            FResourceView.push_back(newView);
        }
    }

    FDataModelWrapper->DirtyVariable("resources");
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
