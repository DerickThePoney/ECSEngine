#include "stdafx.h"

#include "SceneActions.h"

#include "Common/SavingSystemImplementation.h"
#include "PropertyDrawer.h"
#include "RenderingCore/DrawCommands.h"

namespace ECSEngine
{
IMPLEMENT_VIRTUAL_SAVELOAD_ABILITIES(ISceneAction);
template<typename Chunk, bool isWriting>
void ISceneAction::SaveLoad(Chunk& parChunk)
{
    parChunk& FStarted;
    parChunk& FFinished;
}

ISceneAction::ISceneAction(const std::string& parFName /*= "Dummy"*/)
    : FName(parFName)
    , FScene(nullptr)
    , FStarted(false)
    , FFinished(false)
    , FShowEditor(false)
#ifdef PERFORM_SECURITY_CHECKS
    , FVirtualInitialiseCalled(false)
    , FVirtualShutdownCalled(false)
    , FVirtualStartCalled(false)
    , FVirtualUpdateCalled(false)
    , FVirtualFinishCalled(false)
    , FVirtualDrawEditorCalled(false)
    , FVirtualDrawInSceneEditorCalled(false)
#endif
{
}

ISceneAction::~ISceneAction()
{
}

void ISceneAction::Initialise(const SceneScenario* parScene)
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitialiseCalled = false;
#endif
    VirtualInitialise(parScene);

    AlwaysCheckedAssertMsg(FVirtualInitialiseCalled, "A call to VirtualInitialise of the parent was forgotten");
}

void ISceneAction::Shutdown()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualShutdownCalled = false;
#endif
    VirtualShutdown();

    AlwaysCheckedAssertMsg(FVirtualShutdownCalled, "Un appel à VirtualShutdown du parent à été oublié");
}

void ISceneAction::Start()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualStartCalled = false;
#endif
    VirtualStart();

    AlwaysCheckedAssertMsg(FVirtualStartCalled, "Un appel à VirtualStart du parent à été oublié");
}

void ISceneAction::Update()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualUpdateCalled = false;
#endif
    VirtualUpdate();

    AlwaysCheckedAssertMsg(FVirtualUpdateCalled, "Un appel à VirtualUpdate du parent à été oublié");
}

void ISceneAction::Finish()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualFinishCalled = false;
#endif
    VirtualFinish();

    AlwaysCheckedAssertMsg(FVirtualFinishCalled, "Un appel à VirtualFinish du parent à été oublié");
}

bool ISceneAction::IsStarted() const
{
    return FStarted;
}

bool ISceneAction::IsFinished() const
{
    return FFinished;
}

void ISceneAction::DrawEditor()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDrawEditorCalled = false;
#endif
    ImGui::PushID(ImGui::GetID(this));
    VirtualDrawEditor();
    ImGui::PopID();

    AlwaysCheckedAssertMsg(FVirtualDrawEditorCalled, "Un appel à VirtualDrawEditor du parent à été oublié");
}

bool ISceneAction::DrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDrawEditorCalled = false;
#endif
    const bool res = VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);
    AlwaysCheckedAssertMsg(FVirtualDrawInSceneEditorCalled, "Un appel à VirtualDrawInSceneEditor du parent à été oublié");
    return res;
}

void ISceneAction::VirtualInitialise(const SceneScenario* parScene)
{
    FScene = parScene;
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitialiseCalled = true;
#endif
}

void ISceneAction::VirtualShutdown()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualShutdownCalled = true;
#endif
}

void ISceneAction::VirtualStart()
{
    AlwaysCheckedAssert(!FStarted);
    AlwaysCheckedAssert(!FFinished);
    AssertRelease(FScene != nullptr);
    FStarted = true;
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualStartCalled = true;
#endif
}

void ISceneAction::VirtualUpdate()
{
    AlwaysCheckedAssert(FStarted && !FFinished);

#ifdef PERFORM_SECURITY_CHECKS
    FVirtualUpdateCalled = true;
#endif
}

void ISceneAction::VirtualFinish()
{
    AlwaysCheckedAssert(FStarted);
    FFinished = true;
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualFinishCalled = true;
#endif
}

void ISceneAction::VirtualDrawEditor()
{
    FShowEditor = ImGui::CollapsingHeader("", ImGuiTreeNodeFlags_CollapsingHeader);
    ImGui::PushID(ImGui::GetID(this));
    ImGui::SameLine();
    ImGui::Text("%s", FName.c_str());

    if (FShowEditor)
    {
        EDITOR_PROPERTY_STRING("Scene action name", FName, false, "");
    }

#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDrawEditorCalled = true;
#endif
    ImGui::PopID();
}

bool ISceneAction::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDrawInSceneEditorCalled = true;
#endif
    return false;
}

} // namespace ECSEngine
