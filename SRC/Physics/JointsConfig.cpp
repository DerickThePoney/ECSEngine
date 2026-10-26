#include "stdafx.h"

#include "JointsConfig.h"

namespace ECSEngine
{
namespace Physics
{

void JointConfig::DrawInEditor()
{
    EDITOR_PROPERTY_SIMPLE("Anchor A Local", FAnchorALocal);
    EDITOR_PROPERTY_SIMPLE("Anchor B Local", FAnchorBLocal);
}
void DistanceJointConfig::DrawInEditor()
{
    FCommon.DrawInEditor();
    EDITOR_PROPERTY_SIMPLE("Length", FLength);
}
} // namespace Physics
} // namespace ECSEngine