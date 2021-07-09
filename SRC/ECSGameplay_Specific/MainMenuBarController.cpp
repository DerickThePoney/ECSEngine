#include "stdafx.h"

#include "MainMenuBarController.h"

#include "UICore/RMLUIManager.h"

#ifndef RMLUI_STATIC_LIB
#define RMLUI_STATIC_LIB
#endif
#include "Common/Logger.h"
#include "Common/TimeManager.h"

#include <RmlUi/Core/DataModelHandle.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/EventListenerInstancer.h>
#include <RmlUi/Core/Factory.h>

namespace ECSEngine
{
namespace UI
{
class TestListener : public Rml::EventListener
{
public:
    TestListener(const Rml::String& value, Rml::Element* element)
        : FValue(value)
        , FElement(element)
    {
    }
    virtual void ProcessEvent(Rml::Event& event) override
    {
        AlwaysCheckedAssert(event.GetId() == Rml::EventId::Click);
        event.StopImmediatePropagation();
    }

private:
    std::string FValue;
    Rml::Element* FElement;
};

class TestListenerInstancer : public Rml::EventListenerInstancer
{
public:
    virtual Rml::EventListener* InstanceEventListener(const Rml::String& value, Rml::Element* element) override { return new TestListener(value, element); }
};

std::unique_ptr<TestListenerInstancer> listener = std::unique_ptr<TestListenerInstancer>(new TestListenerInstancer);

MainMenuBarController::MainMenuBarController()
{
}

MainMenuBarController::~MainMenuBarController()
{
}

void MainMenuBarController::VirtualInit()
{
    UIControllerWithModuleAccessors::VirtualInit();

    Rml::Factory::RegisterEventListenerInstancer(listener.get());
    Rml::DataModelConstructor ctr = RmlUiManager::Instance().CreateDataModel("colony-model");
    AssertRelease((bool)ctr);
    ctr.Bind("peons", &FModel.TotalPeons);
    ctr.Bind("idle", &FModel.IdlePeons);
    ctr.Bind("remaining_feeding_time", &FModel.RemainingFeedingTime);

    FDataModelWrapper = RmlDataModelWrapperFactory::CreateDataModelWrapper(ctr.GetModelHandle());

    FDocument = RmlUiManager::Instance().LoadDocument("UI\\MainMenuBar\\MainMenuBar.rml");

    FDocumentDemo = RmlUiManager::Instance().LoadDocument("UI\\DemoWindow\\tutorial.rml");
    if (FDocumentDemo)
    {
        FDocumentDemo->GetElementById("title")->SetInnerRML(FDocumentDemo->GetTitle());
    }
}

void MainMenuBarController::VirtualUpdate()
{
    UIControllerWithModuleAccessors::VirtualUpdate();
    if (!HandleVisibility())
        return;

    const EntityId colonyId = EntityId(Worlds::COLONY, 0);

    const ColonyModule* colonyModule = GetModule<ColonyModule>(colonyId);
    if (colonyModule == nullptr)
        return;

    const ResourceStorageModule* resourceStorage = GetModule<ResourceStorageModule>(colonyId);
    AssertRelease(resourceStorage != nullptr);

    const ColonyPeonsManagementModule* peonManagerModule = GetModule<ColonyPeonsManagementModule>(colonyId);
    AssertRelease(peonManagerModule != nullptr);

    PeonSpawnModule* peonSpawnModule = GetModule<PeonSpawnModule>(colonyId);
    AssertRelease(peonSpawnModule != nullptr);

    const PeonFeedingTimeModule* peonFeedingTimeModule = GetModule<PeonFeedingTimeModule>(colonyId);

    FModel.TotalPeons = peonManagerModule->PeonsInColony();
    FModel.IdlePeons = peonManagerModule->IdlePeons().size();
    FModel.RemainingFeedingTime = peonFeedingTimeModule->RemainingTimeBeforeNextFeed();
    FDataModelWrapper->DirtyVariable("peons");
    FDataModelWrapper->DirtyVariable("idle");
    FDataModelWrapper->DirtyVariable("remaining_feeding_time");

    const float progress = peonFeedingTimeModule->RemainTimeBeforeNextFeedAsRatio();
    Rml::Element* progressBar = FDocument->GetElementById("progress");
    AssertRelease(progressBar != nullptr);
    progressBar->SetAttribute("value", progress);
}

void MainMenuBarController::VirtualDestroy()
{
    UIControllerWithModuleAccessors::VirtualDestroy();
    if (FDocument != nullptr)
    {
        RmlUiManager::Instance().UnloadDocument(FDocument);
        RmlUiManager::Instance().UnloadDocument(FDocumentDemo);
    }
    RmlUiManager::Instance().RemoveDataModel("colony-model");
}

bool MainMenuBarController::HandleVisibility()
{
    const bool visible = FDocument->IsVisible();
    if (FShow && !visible)
    {
        FDocument->Show();
        FDocumentDemo->Show();
    }
    else if (!FShow && visible)
    {
        FDocument->Hide();
        FDocumentDemo->Hide();
    }
    return FShow;
}

} // namespace UI
} // namespace ECSEngine