
#include "stdafx.h"

#include "RocketManagerModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "ECSGameplay_Common/PhysicsJointSpawner.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "Physics/PhysicsAPI.h"

CEREAL_REGISTER_TYPE(ECSEngine::RocketManagerModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::RocketManagerModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(RocketManagerModule, RocketManagerModuleTemplate);

Module* RocketManagerModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<RocketManagerModule>(this, parUnitId, parParameters);
}

void RocketManagerModuleTemplate::VirtualDrawEditor()
{
    const auto OldTemplateBody = FRocketBodyTemplateName;
    EDITOR_PROPERTY_ENTITY_TEMPLATE("Rocket body", FRocketBodyTemplate, FRocketBodyTemplateName);
    if (FRocketBodyTemplateName != OldTemplateBody)
    {
        FRocketBodyTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FRocketBodyTemplateName);
    }

    const auto OldTemplateNozzle = FRocketNozzleTemplateName;
    EDITOR_PROPERTY_ENTITY_TEMPLATE("Rocket Nozzle", FRocketNozzleTemplate, FRocketNozzleTemplateName);
    if (FRocketNozzleTemplateName != OldTemplateNozzle)
    {
        FRocketNozzleTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FRocketNozzleTemplateName);
    }

    const auto OldTemplateCamera = FRocketCameraMoverTemplateName;
    EDITOR_PROPERTY_ENTITY_TEMPLATE("Rocket Camera Mover", FRocketCameraMoverTemplate, FRocketCameraMoverTemplateName);
    if (FRocketCameraMoverTemplateName != OldTemplateNozzle)
    {
        FRocketCameraMoverTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FRocketCameraMoverTemplateName);
    }

    FBallConfig.DrawInEditor();
}

void RocketManagerModuleTemplate::VirtualPostLoad()
{
    FRocketBodyTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FRocketBodyTemplateName);
    FRocketNozzleTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FRocketNozzleTemplateName);
    FRocketCameraMoverTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FRocketCameraMoverTemplateName);
}

IMPLEMENT_SAVELOAD_ABILITIES(RocketManagerModule);
template<typename Chunk, bool isWriting>
void RocketManagerModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
}

RocketManagerModule::RocketManagerModule()
    : Module()
    , JointCreatedDelegate(DELEGATE(&RocketManagerModule::OnJointCreated, *this))
{
}

RocketManagerModule::~RocketManagerModule()
{
}

void RocketManagerModule::SetSASActivated(const bool Value)
{
    FWasSASActivated = FSASActivated;
    FSASActivated = Value;
    if (FWasSASActivated && !FSASActivated)
    {
        ClearSASTarget();
    }
}

void RocketManagerModule::SetSASTarget(const quat& Target)
{
    FSASTarget = Target;
    FHasSASTarget = true;
}

void RocketManagerModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    vec3 Position = parParameters.Get_IFP<ModuleParameters::Position>(vec3{ });
    vec3 EulerAngles = parParameters.Get_IFP<ModuleParameters::EulerAnglesXYZ>(vec3{ });

    const RocketManagerModuleTemplate* moduleTemplate = Template<RocketManagerModuleTemplate>();

    // Create the body
    ModuleParameters::ParameterContainer container = parParameters;
    FBodyId = EntityFactory::RequestCreateEntity(moduleTemplate->GetBodyTemplate(), container);

    // compute nozzle position
    const Physics::BallJointConfig& JointConfig = moduleTemplate->GetJointConfig();
    const mat4 R = EulerAnglesXYZ(EulerAngles.x, EulerAngles.y, EulerAngles.z); // rotation only
    const vec3 n = Normalize((R * vec4::MakeHomogeneousDirectionVec4(vec3(0.f, -1.f, 0.f))).xyz());
    const vec3 NozzlePosition = Position + (R * vec4::MakeHomogeneousDirectionVec4(JointConfig.FCommon.FAnchorALocal - JointConfig.FCommon.FAnchorBLocal)).xyz();
    container.Set<ModuleParameters::Position>(NozzlePosition);

    FNozzleId = EntityFactory::RequestCreateEntity(moduleTemplate->GetNozzleTemplate(), container);

    PhysicsJointSpawner::Instance().AddBallJointToSpawn(FBodyId, FNozzleId, JointConfig, &JointCreatedDelegate);

    // Create camera mover template
    ModuleParameters::ParameterContainer CamContainer = parParameters;
    CamContainer.Set<ModuleParameters::TargetId>(parUnitId);
    EntityFactory::RequestCreateEntity(moduleTemplate->GetRocketCameraMoverTemplate(), CamContainer);
}

void RocketManagerModule::OnJointCreated(const Physics::PhysicsJointHandle& Handle)
{
    FJointHandle = Handle;
}

template<typename RigidbodyAccessor>
bool RocketManagerModule::ComputeCenterOfMass(const RigidbodyAccessor& Rigidbodies, vec3& OutPosition, vec3* OutVelocity) const
{
    const EntityId partIds[] = { FBodyId, FNozzleId };

    vec3 weightedPosition{ 0.f };
    vec3 weightedVelocity{ 0.f };
    float totalMass = 0.f;

    for (const EntityId& partId : partIds)
    {
        const RigidbodyModule* const rigidbody = Rigidbodies[partId];
        if (rigidbody == nullptr)
        {
            return false;
        }

        const Physics::PhysicsBodyHandle& bodyHandle = rigidbody->BodyHandle();
        vec3 centerOfMass;
        float mass = 0.f;
        if (!Physics::GetBodyCenterOfMass(bodyHandle, centerOfMass) || !Physics::GetBodyMass(bodyHandle, mass))
        {
            return false;
        }

        weightedPosition = weightedPosition + centerOfMass * mass;
        totalMass += mass;

        if (OutVelocity != nullptr)
        {
            vec3 velocity;
            if (!Physics::GetBodyVelocity(bodyHandle, velocity))
            {
                return false;
            }
            weightedVelocity = weightedVelocity + velocity * mass;
        }
    }

    if (totalMass <= 0.f)
    {
        return false;
    }

    const float inverseTotalMass = 1.f / totalMass;
    OutPosition = weightedPosition * inverseTotalMass;
    if (OutVelocity != nullptr)
    {
        *OutVelocity = weightedVelocity * inverseTotalMass;
    }
    return true;
}

template bool RocketManagerModule::ComputeCenterOfMass<ModuleAccessor<RigidbodyModule>>(const ModuleAccessor<RigidbodyModule>&, vec3&, vec3*) const;
template bool RocketManagerModule::ComputeCenterOfMass<ManualLockModuleAccessor<RigidbodyModule>>(const ManualLockModuleAccessor<RigidbodyModule>&, vec3&, vec3*) const;

} // namespace ECSEngine
