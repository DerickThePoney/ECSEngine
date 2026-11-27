#include "stdafx.h"

#include "RocketPartRuntime.h"

#include "ECSCore/EntityFactory.h"
#include "ECSCore/ModuleParameters.h"

namespace ECSEngine
{

void RocketPart::RefreshCb()
{
    Cb = DELEGATE(&RocketPart::OnJointCreated, *this);
}

void RocketPart::OnJointCreated(const Physics::PhysicsJointHandle& Handle)
{
    FJointHandle = Handle;
}

namespace RocketRuntime
{

bool BuildRocketRuntime(RocketPartGroup& OutGroup, const RocketRecipe& Recipe, const vec3& Position, const vec3& EulerAngles)
{
    OutGroup.FParts.clear();
    OutGroup.FParts.reserve(Recipe.FRocketPartRecipes.size());

    ModuleParameters::ParameterContainer parameters;
    parameters.Set<ModuleParameters::EulerAnglesXYZ>(EulerAngles);
    vec3 CurrentPosition = Position;
    forrange(i, 0, Recipe.FRocketPartRecipes.size())
    {
        const RocketPartRecipe& partRecipe = Recipe.FRocketPartRecipes[i];

        if (i > 0 && partRecipe.FJointType != ERocketPartJoint::NONE)
        {
            const Physics::JointConfig* JointConfig = &partRecipe.FBallConfig.FCommon;
            if (partRecipe.FJointType == ERocketPartJoint::WELD)
            {
                JointConfig = &partRecipe.FWeldConfig.FCommon;
            }

            const mat4 R = EulerAnglesXYZ(EulerAngles.x, EulerAngles.y, EulerAngles.z); // rotation only
            CurrentPosition = CurrentPosition + (R * vec4::MakeHomogeneousDirectionVec4(JointConfig->FAnchorALocal - JointConfig->FAnchorBLocal)).xyz();
        }

        parameters.Set<ModuleParameters::Position>(CurrentPosition);

        RocketPart& part = OutGroup.FParts.emplace_back(RocketPart{ .FJointType = partRecipe.FJointType,
              .FbDecoupler = partRecipe.FbDecoupler,
              .FSeparationSpeed = partRecipe.FSeparationSpeed,
              .FRole = partRecipe.FRole,
              .FFuelTankIndex = partRecipe.FFuelTankIndex });
        part.RefreshCb();

        AssertRelease(partRecipe.FEntityTemplate != nullptr);
        part.FId = EntityFactory::RequestCreateEntity(partRecipe.FEntityTemplate, parameters);
        AssertRelease(part.FId.Valid());

        if (i > 0)
        {
            part.FParent = i - 1;
            i32 ParentIndex = part.FParent;
            if (OutGroup.FParts[ParentIndex].FRole == ERocketPartRole::ENGINE)
                ParentIndex = OutGroup.FParts[ParentIndex].FFuelTankIndex;

            const EntityId ParentId = OutGroup.FParts[ParentIndex].FId;
            switch (part.FJointType)
            {
            case ERocketPartJoint::BALL:
                PhysicsJointSpawner::Instance().AddBallJointToSpawn(ParentId, part.FId, partRecipe.FBallConfig, &part.Cb);
                break;
            case ERocketPartJoint::WELD:
                PhysicsJointSpawner::Instance().AddWeldJointToSpawn(ParentId, part.FId, partRecipe.FWeldConfig, &part.Cb);
                break;
            }
        }
    }

    return true;
}

} // namespace RocketRuntime

// namespace RocketRuntime
} // namespace ECSEngine