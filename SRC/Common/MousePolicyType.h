#pragma once
namespace ECSEngine
{
namespace MousePolicyType
{
#define DECLARE_MOUSE_POLICY_TYPE(X) X,
enum Type
{
#include "MousePolicyType.inl"
    LENGTH
};
#undef DECLARE_MOUSE_POLICY_TYPE
} // namespace MousePolicyType
} // namespace ECSEngine
