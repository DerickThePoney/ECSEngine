#pragma once

// TODO
// - POOL Allocation
// - Type id comme les sceneitems et les modules
// - Factories

namespace ECSEngine
{
/*************************************************************/
/*                      ISceneAction                         */
/*************************************************************/
class SceneScenario;

namespace Rendering
{
class DrawCommandBuffer;
class MaterialInstanceHandle;
} // namespace Rendering

class ISceneAction
{
    friend class SceneScenario;

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

    const SceneScenario* GetScene() const { return FScene; }
    void SetScene(const SceneScenario* parScene) { FScene = parScene; }

    void Initialise(const SceneScenario* parScene);
    void Shutdown();

    void Start();
    void Update();
    void Finish();

    bool IsStarted() const;
    bool IsFinished() const;

    void DrawEditor();
    bool ShouldShowEditor() const { return FShowEditor; }
    bool DrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial);

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(Name), PROPERTY(Started), PROPERTY(Finished));
    }

protected:
    virtual void VirtualInitialise(const SceneScenario* parScene);
    virtual void VirtualShutdown();

    virtual void VirtualStart();
    virtual void VirtualUpdate();
    virtual void VirtualFinish();

    virtual void VirtualDrawEditor();
    virtual bool VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial);

private:
    std::string FName;
    const SceneScenario* FScene;

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
    bool FVirtualDrawInSceneEditorCalled;
#endif
};

} // namespace ECSEngine

#include "SceneActionIds.h"

#define DECLARE_SCENE_ACTION(TYPE, PARENT_TYPE)                                                                                                                                    \
    DECLARE_POOL_ALLOCATED(TYPE);                                                                                                                                                  \
    using parent_type = PARENT_TYPE;                                                                                                                                               \
                                                                                                                                                                                   \
public:                                                                                                                                                                            \
    static u32 GetSceneActionTypeId() { return SceneActionTrait<TYPE>::GetSceneActionTypeId(); }                                                                                   \
    const std::string GetTypeName() const override { return #TYPE; }

#define IMPLEMENT_SCENE_ACTION(TYPE)                                                                                                                                               \
    IMPLEMENT_POOL_ALLOCATED(TYPE);                                                                                                                                                \
    ISceneAction* CreateAction##TYPE() { return new TYPE; }                                                                                                                        \
    static bool registered##TYPE = SceneActionManagement::RegisterSceneActionFactory(TYPE::GetSceneActionTypeId(), &CreateAction##TYPE);
