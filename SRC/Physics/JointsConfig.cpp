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

    EDITOR_PROPERTY_BOOL("Enable Motor", FEnableMotor);
    if (FEnableMotor)
    {
        EDITOR_PROPERTY_BOOL("Use target angle", FUseTargetAngle);
        if (FUseTargetAngle)
        {
            EDITOR_PROPERTY_ANGLE("Target Angle", FTargetAngle, -180.f, 180.f);
            EDITOR_PROPERTY_WITH_LIMITS("Motor Stiffness", FMotorStiffness, 0.f, 100.f);
        }
        else
        {
            EDITOR_PROPERTY_WITH_LIMITS("Motor Speed", FMotorSpeed, -20.f, 20.f);
        }

        EDITOR_PROPERTY_WITH_LIMITS("Max Motor Torque", FMaxMotorTorque, 100.f, 1000.f);
    }
}

} // namespace Physics
} // namespace ECSEngine