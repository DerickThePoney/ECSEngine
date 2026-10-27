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
        EDITOR_PROPERTY_ANGLE("Lower angle", FLowerAngle, -Pi(), Pi());
        EDITOR_PROPERTY_ANGLE("Upper angle", FUpperAngle, FLowerAngle, Pi());
    }
}

} // namespace Physics
} // namespace ECSEngine