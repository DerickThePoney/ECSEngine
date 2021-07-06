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

class RmlDataModelWrapper final : public IDataModelWrapper
{
public:
    RmlDataModelWrapper(Rml::DataModelHandle parHandle)
        : FHandle(parHandle)
    {
    }

    bool IsVariableDirty(const std::string& variable_name) override { return FHandle.IsVariableDirty(variable_name); }

    void DirtyVariable(const std::string& variable_name) override { FHandle.DirtyVariable(variable_name); }
    explicit operator bool() override { return (bool)FHandle; }

private:
    Rml::DataModelHandle FHandle;
};

class TestListener : public Rml::EventListener
{
public:
    virtual void ProcessEvent(Rml::Event& event) override
    {
        AlwaysCheckedAssert(event.GetId() == Rml::EventId::Click);
        auto value = event.GetTargetElement()->GetAttribute("onclick");
        event.StopImmediatePropagation();
    }
};

std::unique_ptr<TestListener> listener = std::unique_ptr<TestListener>(new TestListener);

MainMenuBarController::MainMenuBarController()
{
}

MainMenuBarController::~MainMenuBarController()
{
}

void MainMenuBarController::VirtualInit()
{
    UIControllerWithModuleAccessors::VirtualInit();

    Rml::DataModelConstructor ctr = RmlUiManager::Instance().CreateDataModel("colony-model");
    AssertRelease((bool)ctr);
    ctr.Bind("peons", &FModel.TotalPeons);
    ctr.Bind("idle", &FModel.IdlePeons);

    FDataModelWrapper.reset(new RmlDataModelWrapper(ctr.GetModelHandle()));

    FDocument = RmlUiManager::Instance().LoadDocument("UI\\MainMenuBar\\MainMenuBar.rml");
    FDocument->AddEventListener("click", listener.get());
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

    FModel.TotalPeons = peonManagerModule->PeonsInColony();
    FModel.IdlePeons = peonManagerModule->IdlePeons().size();
    FDataModelWrapper->DirtyVariable("peons");
    FDataModelWrapper->DirtyVariable("idle");

    static float value = 0.f;
    static float direction = 1.f;

    value += direction * 5 * TimeManager::FrameDeltaTime();
    if (value > 10.f)
    {
        direction = -1.f;
        value = 10.f;
    }

    if (value < 0.f)
    {
        direction = 1.f;
        value = 0.f;
    }

    Rml::Element* progressBar = FDocument->GetElementById("progress");
    AssertRelease(progressBar != nullptr);
    progressBar->SetAttribute("value", value);
}

void MainMenuBarController::VirtualDestroy()
{
    UIControllerWithModuleAccessors::VirtualDestroy();
    if (FDocument != nullptr)
    {
        FDocument->RemoveEventListener("click", listener.get());
        RmlUiManager::Instance().UnloadDocument(FDocument);
    }
    RmlUiManager::Instance().RemoveDataModel("colony-model");
}

bool MainMenuBarController::HandleVisibility()
{
    const bool visible = FDocument->IsVisible();
    if (FShow && !visible)
        FDocument->Show();
    else if (!FShow && visible)
        FDocument->Hide();
    return FShow;
}

} // namespace UI
} // namespace ECSEngine