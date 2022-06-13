#pragma once

namespace ECSEngine
{
namespace UI
{

class UIController
{
public:
    virtual ~UIController();

    void Init();

    virtual void Update();

    void Destroy();

    void Show(bool parShow) { FShow = parShow; }
    bool Shown() const { return FShow; }

protected:
    virtual void VirtualInit();
    virtual void VirtualUpdate();
    virtual void VirtualDestroy();

protected:
    bool FShow = false;

#ifdef PERFORM_SECURITY_CHECKS
    bool FVirtualInitCalled;
    bool FVirtualUpdateCalled;
    bool FVirtualDestroyCalled;
#endif
};

} // namespace UI
} // namespace ECSEngine
