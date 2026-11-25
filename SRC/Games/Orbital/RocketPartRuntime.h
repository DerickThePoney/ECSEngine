#pragma once
#include "ECSCore/EntityId.h"
#include "ECSGameplay_Common/PhysicsJointSpawner.h"
#include "Physics/PhysicsJointHandle.h"
#include "RocketRecipe.h"

namespace ECSEngine
{
struct RocketPart
{
    EntityId FId = EntityId();
    i32 FParent = -1;
    Physics::PhysicsJointHandle FJointHandle;

    ERocketPartJoint FJointType = ERocketPartJoint::NONE;
    bool FbDecoupler = false;
    float FSeparationSpeed = 0.f;
    ERocketPartRole FRole = ERocketPartRole::NONE;
    i32 FFuelTankIndex = -1;

    OnJointCreatedCallback Cb = DELEGATE(&RocketPart::OnJointCreated, *this);

    void RefreshCb();
    void OnJointCreated(const Physics::PhysicsJointHandle& Handle);
};

struct RocketPartGroup
{
    std::vector<RocketPart> FParts;
};

namespace RocketRuntime
{
bool BuildRocketRuntime(RocketPartGroup& OutGroup, const RocketRecipe& Recipe, const vec3& Position, const vec3& EulerAngles);
}
} // namespace ECSEngine
