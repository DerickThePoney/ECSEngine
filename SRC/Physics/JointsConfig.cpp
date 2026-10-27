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

void BallJointConfig::DrawInEditor()
{
    FCommon.DrawInEditor();
}

void HingeJointConfig::DrawInEditor()
{
    FCommon.DrawInEditor();

    EDITOR_PROPERTY_SIMPLE("Hinge axis local on A", FAxisALocal);
    EDITOR_PROPERTY_BOOL("Enable limits", FEnableLimit);

    if (FEnableLimit)
    {
        EDITOR_PROPERTY_ANGLE("Lower angle", FLowerAngle, -180.f, 180.f);
        EDITOR_PROPERTY_ANGLE("Upper angle", FUpperAngle, FLowerAngle, 180.f);
    }
}

} // namespace Physics
} // namespace ECSEngine