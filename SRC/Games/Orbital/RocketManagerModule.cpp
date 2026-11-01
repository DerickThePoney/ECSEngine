
#include "stdafx.h"

#include "RocketManagerModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSGameplay_Common/PhysicsJointSpawner.h"

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

    FBallConfig.DrawInEditor();
}

void RocketManagerModuleTemplate::VirtualPostLoad()
{
    FRocketBodyTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FRocketBodyTemplateName);
    FRocketNozzleTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FRocketNozzleTemplateName);
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
}

void RocketManagerModule::OnJointCreated(const Physics::PhysicsJointHandle& Handle)
{
    FJointHandle = Handle;
}

} // namespace ECSEngine
