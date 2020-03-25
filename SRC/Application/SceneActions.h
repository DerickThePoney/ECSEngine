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
class ISceneAction
{
protected:
    ISceneAction(const std::string& parFName = "Dummy");
    virtual ~ISceneAction();

public:
    const std::string& GetName() const { return FName; }
    void SetName(const std::string& parName) { FName = parName; }

    void Initialise();
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
        ar(PROPERTY(Name), PROPERTY(FStarted), PROPERTY(FFinished));
    }

protected:
    virtual void VirtualInitialise();
    virtual void VirtualShutdown();

    virtual void VirtualStart();
    virtual void VirtualUpdate();
    virtual void VirtualFinish();

    virtual void VirtualDrawEditor();

private:
    std::string FName;

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
