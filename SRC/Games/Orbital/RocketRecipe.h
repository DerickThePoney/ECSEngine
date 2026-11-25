#pragma once
#include "Physics/JointsConfig.h"

namespace ECSEngine
{
class EntityTemplate;

enum class ERocketPartJoint : u8
{
    NONE,
    BALL,
    WELD
};

enum class ERocketPartRole : u8
{
    NONE,
    FUEL_TANK,
    ENGINE
};

struct RocketPartRecipe
{
    std::string FEntityTemplateName;
    const EntityTemplate* FEntityTemplate = nullptr;

    ERocketPartJoint FJointType = ERocketPartJoint::NONE;
    Physics::BallJointConfig FBallConfig;
    Physics::WeldJointConfig FWeldConfig;

    bool FbDecoupler = false;
    float FSeparationSpeed = 2.f;

    ERocketPartRole FRole = ERocketPartRole::NONE;
    i32 FFuelTankIndex = -1;

    void FetchEntityTemplate();

    SERIALIZE()
    {
        PROPERTYFIELD(EntityTemplateName, "");
        PROPERTYFIELD(JointType, ERocketPartJoint::NONE);
        PROPERTYFIELD(BallConfig, Physics::BallJointConfig{ });
        PROPERTYFIELD(WeldConfig, Physics::WeldJointConfig{ });
        PROPERTYFIELD(bDecoupler, false);
        PROPERTYFIELD(SeparationSpeed, 2.f);
        PROPERTYFIELD(Role, ERocketPartRole::NONE);
        PROPERTYFIELD(FuelTankIndex, -1);

        FetchEntityTemplate();
    }

    void DrawInEditor();
};

struct RocketRecipe
{
    std::vector<RocketPartRecipe> FRocketPartRecipes;

    SERIALIZE() { PROPERTYFIELD(RocketPartRecipes, std::vector<RocketPartRecipe>()); }
};
} // namespace ECSEngine
