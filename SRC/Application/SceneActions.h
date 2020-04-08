#pragma once

#include "SceneActionManagement.h"
// TODO
// - POOL Allocation
// - Type id comme les sceneitems et les modules
// - Factories

namespace ECSEngine
{
/*************************************************************/
/*                      ISceneAction                         */
/*************************************************************/
class Scene;
class ISceneAction
{
    friend class Scene;

public:
    ISceneAction(const std::string& parFName = "Dummy");
    virtual ~ISceneAction();

public:
    virtual const std::string GetTypeName() const
    {
        AssertNotReached();
        return "Error";
    }
    const std::string& GetName() const { return FName; }
    void SetName(const std::string& parName) { FName = parName; }

    const Scene* GetScene() const { return FScene; }
    void SetScene(const Scene* parScene) { FScene = parScene; }

    void Initialise(const Scene* parScene);
    void Shutdown();

    void Start();
    void Update();
    void Finish();

    bool IsStarted() const;
    bool IsFinished() const;

    void DrawEditor();
    bool ShouldShowEditor() const { return FShowEditor; }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(Name), PROPERTY(Started), PROPERTY(Finished));
    }

protected:
    virtual void VirtualInitialise(const Scene* parScene);
    virtual void VirtualShutdown();

    virtual void VirtualStart();
    virtual void VirtualUpdate();
    virtual void VirtualFinish();

    virtual void VirtualDrawEditor();

private:
    std::string FName;
    const Scene* FScene;

    bool FStarted;
    bool FFinished;

    bool FShowEditor;

#ifdef PERFORM_SECURITY_CHECKS
    bool FVirtualInitialiseCalled;
    bool FVirtualShutdownCalled;

    bool FVirtualStartCalled;
    bool FVirtualUpdateCalled;
    bool FVirtualFinishCalled;

    bool FVirtualDrawEditorCalled;
#endif
};

} // namespace ECSEngine

#include "SceneActionIds.h"

#define DECLARE_SCENE_ACTION(TYPE)                                                                                                                                                 \
    DECLARE_POOL_ALLOCATED(TYPE);                                                                                                                                                  \
                                                                                                                                                                                   \
public:                                                                                                                                                                            \
    static u32 GetSceneActionTypeId() { return SceneActionTrait<TYPE>::GetSceneActionTypeId(); }                                                                                   \
    const std::string GetTypeName() const override { return #TYPE; }

#define IMPLEMENT_SCENE_ACTION(TYPE)                                                                                                                                               \
    IMPLEMENT_POOL_ALLOCATED(TYPE);                                                                                                                                                \
    ISceneAction* CreateAction##TYPE() { return new TYPE; }                                                                                                                        \
    static bool registered##TYPE = SceneActionManagement::RegisterSceneActionFactory(TYPE::GetSceneActionTypeId(), &CreateAction##TYPE);
