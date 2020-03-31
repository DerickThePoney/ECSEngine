#include "stdafx.h"

#include "SceneActions.h"

#include "PropertyDrawer.h"

namespace ECSEngine
{

ISceneAction::ISceneAction(const std::string& parFName /*= "Dummy"*/)
    : FName(parFName)
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
#endif
{
}

ISceneAction::~ISceneAction()
{
}

void ISceneAction::Initialise()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitialiseCalled = false;
#endif
    VirtualInitialise();

    AlwaysCheckedAssertMsg(FVirtualInitialiseCalled, "Un appel à VirtualInitialise du parent à été oublié");
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

    VirtualDrawEditor();

    AlwaysCheckedAssertMsg(FVirtualDrawEditorCalled, "Un appel à VirtualDrawEditor du parent à été oublié");
}

void ISceneAction::VirtualInitialise()
{
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
    ImGui::CollapsingHeader(FName.c_str(), &FShowEditor);

    if (FShowEditor)
    {
        EDITOR_PROPERTY_STRING("Scene action name", FName, false, "");
    }

#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDrawEditorCalled = true;
#endif
}

} // namespace ECSEngine
