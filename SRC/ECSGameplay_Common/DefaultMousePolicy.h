#pragma once
#include "Common/MousePolicy.h"

namespace ECSEngine
{
class DefaultMousePolicy : public IMousePolicy
{
    MOUSE_POLICY_HEADER(DefaultMousePolicy, IMousePolicy, MousePolicyType::DEFAULT);

public:
};
} // namespace ECSEngine