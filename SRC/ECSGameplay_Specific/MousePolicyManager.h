#pragma once
#include "Common/MousePolicy.h"
#include "Common/Singleton.h"

namespace ECSEngine
{
class MousePolicyManager : public Singleton<MousePolicyManager>
{
public:
    void Initialise();
    void Shutdown();

    void Update();

    void ActivateMousePolicy(const MousePolicyType::Type parType);
    void DeactivateMousePolicy(const MousePolicyType::Type parType);

    template<typename MousePolicyClass>
    MousePolicyClass* GetMousePolicy(const MousePolicyType::Type parType)
    {
        using InterfaceType = std::conditional_t<std::is_const_v<MousePolicyClass>, const IMousePolicy, IMousePolicy>;
        InterfaceType* policy = GetMousePolicy(parType);
        return dynamic_cast<MousePolicyClass*>(policy);
    }

    const IMousePolicy* GetMousePolicy(const MousePolicyType::Type parType) const;
    IMousePolicy* GetMousePolicy(const MousePolicyType::Type parType);

private:
    std::vector<std::unique_ptr<IMousePolicy>> FOrderedMousePolicies;
    std::map<MousePolicyType::Type, u32> FPolicyToIndex;
};
} // namespace ECSEngine
