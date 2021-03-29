#include "stdafx.h"

#include "MousePolicyManager.h"

#include "ECSGameplay_Common/DefaultMousePolicy.h"
#include "PlaceBuildingMousePolicy.h"

namespace ECSEngine
{

void MousePolicyManager::Initialise()
{
    // place building mouse policy
    FOrderedMousePolicies.push_back(std::unique_ptr<IMousePolicy>(new PlaceBuildingMousePolicy()));
    FPolicyToIndex[FOrderedMousePolicies.back()->MousePolicyType()] = (u32)FOrderedMousePolicies.size() - 1u;
    PlaceBuildingMousePolicy* placeBuildingPolicy = GetMousePolicy<PlaceBuildingMousePolicy>(FOrderedMousePolicies.back()->MousePolicyType());
    placeBuildingPolicy->SetupMousePolicy("BuildingTest");
    placeBuildingPolicy->Activate();

    // Default mouse policy
    FOrderedMousePolicies.push_back(std::unique_ptr<IMousePolicy>(new DefaultMousePolicy()));
    FPolicyToIndex[FOrderedMousePolicies.back()->MousePolicyType()] = (u32)FOrderedMousePolicies.size() - 1u;
    // The default mouse policy is always activated
    FOrderedMousePolicies.back()->Activate();
}

void MousePolicyManager::Shutdown()
{
    FOrderedMousePolicies.clear();
}

void MousePolicyManager::Update()
{
    foreachitem(mousePolicy, FOrderedMousePolicies)
    {
        if (mousePolicy->IsActivated())
        {
            mousePolicy->Update();
            // only update the first mouse policy that's activated
            break;
        }
    }
}

void MousePolicyManager::ActivateMousePolicy(const MousePolicyType::Type parType)
{
    auto itFind = FPolicyToIndex.find(parType);
    AlwaysCheckedAssert(itFind != FPolicyToIndex.end());

    if (itFind != FPolicyToIndex.end())
    {
        AssertRelease(FOrderedMousePolicies[itFind->second] != nullptr);
        FOrderedMousePolicies[itFind->second]->Activate();
    }
}

void MousePolicyManager::DeactivateMousePolicy(const MousePolicyType::Type parType)
{
    auto itFind = FPolicyToIndex.find(parType);
    AlwaysCheckedAssert(itFind != FPolicyToIndex.end());

    if (itFind != FPolicyToIndex.end())
    {
        AssertRelease(FOrderedMousePolicies[itFind->second] != nullptr);
        FOrderedMousePolicies[itFind->second]->Deactivate();
    }
}

const IMousePolicy* MousePolicyManager::GetMousePolicy(const MousePolicyType::Type parType) const
{
    auto itFind = FPolicyToIndex.find(parType);
    if (itFind != FPolicyToIndex.end())
    {
        return FOrderedMousePolicies[itFind->second].get();
    }
    return nullptr;
}

IMousePolicy* MousePolicyManager::GetMousePolicy(const MousePolicyType::Type parType)
{
    auto itFind = FPolicyToIndex.find(parType);
    if (itFind != FPolicyToIndex.end())
    {
        return FOrderedMousePolicies[itFind->second].get();
    }
    return nullptr;
}

} // namespace ECSEngine
