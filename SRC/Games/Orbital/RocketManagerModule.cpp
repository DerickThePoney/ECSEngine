
#include "stdafx.h"

#include "RocketManagerModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/EntityTemplate.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "ECSGameplay_Common/PhysicsJointSpawner.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "FuelTankModule.h"
#include "NozzleEngineModule.h"
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
    if (ImGui::CollapsingHeader("Rocket recipe"))
    {
        FRecipe.DrawInEditor();
    }

    const auto OldTemplateCamera = FRocketCameraMoverTemplateName;
    EDITOR_PROPERTY_ENTITY_TEMPLATE("Rocket Camera Mover", FRocketCameraMoverTemplate, FRocketCameraMoverTemplateName);
    if (FRocketCameraMoverTemplateName != OldTemplateCamera)
    {
        FRocketCameraMoverTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FRocketCameraMoverTemplateName);
    }

    EDITOR_PROPERTY_SIMPLE("Axial drag area", FAxialDragArea);
    EDITOR_PROPERTY_SIMPLE("Broadside drag area", FBroadsideDragArea);
    EDITOR_PROPERTY_SIMPLE("Weathercock", FWeathercock);
}

void RocketManagerModuleTemplate::VirtualPostLoad()
{
    FRocketCameraMoverTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FRocketCameraMoverTemplateName);

    FRecipe.FetchEntityTemplates();

    const std::vector<RocketPartRecipe>& parts = FRecipe.FRocketPartRecipes;
    forrange(i, 0, parts.size())
    {
        const RocketPartRecipe& part = parts[i];
        AssertReleaseMsg(part.FEntityTemplate != nullptr, std::format("Rocket part {} has no entity template", i).c_str());
        if (part.FEntityTemplate == nullptr)
            continue;

        if (i == 0)
        {
            AssertReleaseMsg(part.FJointType == ERocketPartJoint::NONE, "Rocket part 0 must have no joint");
        }
        else
        {
            AssertReleaseMsg(part.FJointType != ERocketPartJoint::NONE, std::format("Rocket part {} must have a joint", i).c_str());
        }

        if (part.FRole == ERocketPartRole::ENGINE)
        {
            AssertReleaseMsg(part.FEntityTemplate->HasModule<NozzleEngineModule>(), std::format("Rocket part {} is an engine without a NozzleEngineModule", i).c_str());

            const bool bTankIndexInRange = part.FFuelTankIndex >= 0 && static_cast<size_t>(part.FFuelTankIndex) < parts.size();
            AssertReleaseMsg(bTankIndexInRange, std::format("Rocket part {} fuel tank index is out of range", i).c_str());
            if (bTankIndexInRange)
            {
                AssertReleaseMsg(parts[part.FFuelTankIndex].FRole == ERocketPartRole::FUEL_TANK, std::format("Rocket part {} does not drain a fuel tank", i).c_str());
            }
        }
        else if (part.FRole == ERocketPartRole::FUEL_TANK)
        {
            AssertReleaseMsg(part.FEntityTemplate->HasModule<FuelTankModule>(), std::format("Rocket part {} is a fuel tank without a FuelTankModule", i).c_str());
        }
    }
}

IMPLEMENT_SAVELOAD_ABILITIES(RocketManagerModule);
template<typename Chunk, bool isWriting>
void RocketManagerModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
}

RocketManagerModule::RocketManagerModule()
    : Module()
{
}

RocketManagerModule::~RocketManagerModule()
{
}

float RocketManagerModule::AxialDragArea() const
{
    const RocketManagerModuleTemplate* const Temp = Template<RocketManagerModuleTemplate>();
    return Temp->AxialDragArea();
}

float RocketManagerModule::BroadsideDragArea() const
{
    const RocketManagerModuleTemplate* const Temp = Template<RocketManagerModuleTemplate>();
    return Temp->BroadsideDragArea();
}

float RocketManagerModule::Weathercock() const
{
    const RocketManagerModuleTemplate* const Temp = Template<RocketManagerModuleTemplate>();
    return Temp->Weathercock();
}

const EntityId RocketManagerModule::ActiveEngine() const
{
    i32 EngineIndex = ActiveEnginePartIndex();
    if (EngineIndex == -1)
        return EntityId();

    return FActive.FParts[EngineIndex].FId;
}

const EntityId RocketManagerModule::ActiveEngineFuelTank() const
{
    i32 EngineIndex = ActiveEnginePartIndex();
    if (EngineIndex == -1)
        return EntityId();

    i32 TankIndex = FActive.FParts[EngineIndex].FFuelTankIndex;
    AssertRelease(TankIndex >= 0 && TankIndex < FActive.FParts.size());
    return FActive.FParts[TankIndex].FId;
}

const Physics::PhysicsJointHandle RocketManagerModule::ActiveEngineJointHandle() const
{
    i32 EngineIndex = ActiveEnginePartIndex();
    if (EngineIndex == -1)
        return Physics::PhysicsJointHandle();

    return FActive.FParts[EngineIndex].FJointHandle;
}

const EntityId RocketManagerModule::RootId() const
{
    AssertRelease(FActive.FParts.size() > 0);
    return FActive.FParts[0].FId;
}

bool RocketManagerModule::Stage()
{
    if (!FbWantsStaging)
        return false;

    i32 DecouplerIndex = NextDecouplerIndex();
    if (DecouplerIndex == -1)
        return false;

    RocketPartGroup& newDebrisGroup = FDebris.emplace_back();
    forrange(i, DecouplerIndex, FActive.FParts.size())
    {
        newDebrisGroup.FParts.push_back(FActive.FParts[i]);
    }
    FActive.FParts.resize(DecouplerIndex);

    RocketPart& RootDebrisPart = newDebrisGroup.FParts[0];
    RootDebrisPart.FParent = -1;

    // Destroy the joint
    Physics::DestroyJoint(RootDebrisPart.FJointHandle);

    // Apply the separation speed

    return true;
}

i32 RocketManagerModule::ActiveEnginePartIndex() const
{
    reverseforrange(i, 0, FActive.FParts.size())
    {
        if (FActive.FParts[i].FRole == ERocketPartRole::ENGINE)
        {
            return i;
        }
    }

    return -1;
}

i32 RocketManagerModule::NextDecouplerIndex() const
{
    reverseforrange(i, 0, FActive.FParts.size())
    {
        if (FActive.FParts[i].FbDecoupler)
        {
            return i;
        }
    }

    return -1;
}

void RocketManagerModule::SetRocketThrottle(const float Value)
{
    FRocketThrottle = Clamp01(Value);
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

    // Create the rocket
    RocketRuntime::BuildRocketRuntime(FActive, moduleTemplate->Recipe(), Position, EulerAngles);

    // Create camera mover template
    ModuleParameters::ParameterContainer CamContainer = parParameters;
    CamContainer.Set<ModuleParameters::TargetId>(parUnitId);
    EntityFactory::RequestCreateEntity(moduleTemplate->GetRocketCameraMoverTemplate(), CamContainer);
}

template<typename RigidbodyAccessor>
void RocketManagerModule::SetPositionAndVelocityForAllPartsUsingOffsets(const RigidbodyAccessor& Rigidbodies,
      const vec3& CoMPosition,
      const vec3& CoMVelocity,
      const MemoryView<const vec3>& parts) const
{
    AssertRelease(parts.size() == FActive.FParts.size());

    forrange(i, 0, FActive.FParts.size())
    {
        const EntityId& partId = FActive.FParts[i].FId;
        const RigidbodyModule* const rigidbody = Rigidbodies[partId];
        AssertRelease(rigidbody != nullptr);

        bool bSuccess = Physics::SetBodyPosition(rigidbody->BodyHandle(), CoMPosition + parts[i]);
        AssertRelease(bSuccess);

        bSuccess = Physics::SetBodyVelocity(rigidbody->BodyHandle(), CoMVelocity);
        AssertRelease(bSuccess);
    }
}

template void RocketManagerModule::SetPositionAndVelocityForAllPartsUsingOffsets<ModuleAccessor<RigidbodyModule>>(const ModuleAccessor<RigidbodyModule>&,
      const vec3&,
      const vec3&,
      const MemoryView<const vec3>&) const;

template<typename RigidbodyAccessor>
void RocketManagerModule::ComputePartOffsetAndClearRotationVelocity(const RigidbodyAccessor& Rigidbodies, const vec3& CoMPosition, MemoryView<vec3>& parts) const
{
    AssertRelease(parts.size() == FActive.FParts.size());

    forrange(i, 0, FActive.FParts.size())
    {
        const EntityId& partId = FActive.FParts[i].FId;
        const RigidbodyModule* const rigidbody = Rigidbodies[partId];
        AssertRelease(rigidbody != nullptr);

        vec3 partPos;
        bool bSuccess = Physics::GetBodyPosition(rigidbody->BodyHandle(), partPos);
        AssertRelease(bSuccess);

        bSuccess = Physics::SetBodyRotationVelocity(rigidbody->BodyHandle(), vec3{ 0.f });
        AssertRelease(bSuccess);

        parts[i] = partPos - CoMPosition;
    }
}

template void RocketManagerModule::ComputePartOffsetAndClearRotationVelocity<ModuleAccessor<RigidbodyModule>>(const ModuleAccessor<RigidbodyModule>&,
      const vec3& CoMPosition,
      MemoryView<vec3>& parts) const;

template<typename RigidbodyAccessor>
bool RocketManagerModule::ComputeCenterOfMass(const RigidbodyAccessor& Rigidbodies, vec3& OutPosition, vec3* OutVelocity) const
{
    vec3 weightedPosition{ 0.f };
    vec3 weightedVelocity{ 0.f };
    float totalMass = 0.f;

    for (const RocketPart& part : FActive.FParts)
    {
        const EntityId& partId = part.FId;
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

template<typename RigidbodyAccessor>
bool RocketManagerModule::ComputeTotalMass(const RigidbodyAccessor& Rigidbodies, float& OutMass) const
{
    OutMass = 0.f;
    foreachitemconst(part, FActive.FParts)
    {
        const EntityId& partId = part.FId;
        const RigidbodyModule* const rigidbody = Rigidbodies[partId];
        AssertRelease(rigidbody != nullptr);

        float Mass = 0.f;
        if (!Physics::GetBodyMass(rigidbody->BodyHandle(), Mass))
        {
            return false;
        }
        OutMass += Mass;
    }
    return true;
}

template bool RocketManagerModule::ComputeTotalMass<ModuleAccessor<RigidbodyModule>>(const ModuleAccessor<RigidbodyModule>&, float& OutMass) const;
template bool RocketManagerModule::ComputeTotalMass<ManualLockModuleAccessor<RigidbodyModule>>(const ManualLockModuleAccessor<RigidbodyModule>&, float& OutMass) const;

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
template<typename RigidbodyAccessor>
bool RocketManagerModule::ApplyForceToAllActiveBodies(const RigidbodyAccessor& Rigidbodies, const vec3& Force, const bool bTreatAsAcceleration) const
{
    return ApplyForceToAllBodiesInGroup(Rigidbodies, FActive, Force, bTreatAsAcceleration);
}

template bool RocketManagerModule::ApplyForceToAllActiveBodies<ModuleAccessor<RigidbodyModule>>(const ModuleAccessor<RigidbodyModule>&, const vec3&, const bool) const;

template<typename RigidbodyAccessor>
bool RocketManagerModule::ApplyForceToAllBodiesInDebrisIndex(const RigidbodyAccessor& Rigidbodies, u32 DebrisIndex, const vec3& Force, const bool bTreatAsAcceleration) const
{
    AssertRelease(DebrisIndex < FDebris.size());
    return ApplyForceToAllBodiesInGroup(Rigidbodies, FDebris[DebrisIndex], Force, bTreatAsAcceleration);
}

template bool RocketManagerModule::ApplyForceToAllBodiesInDebrisIndex<ModuleAccessor<RigidbodyModule>>(const ModuleAccessor<RigidbodyModule>&, u32, const vec3&, const bool) const;

template<typename RigidbodyAccessor>
bool RocketManagerModule::ApplyForceToAllBodiesInGroup(const RigidbodyAccessor& Rigidbodies,
      const RocketPartGroup& RocketPartGroup,
      const vec3& Force,
      const bool bTreatAsAcceleration) const
{
    for (const RocketPart& part : RocketPartGroup.FParts)
    {
        const EntityId& partId = part.FId;
        const RigidbodyModule* const rigidbody = Rigidbodies[partId];
        AssertRelease(rigidbody != nullptr);

        if (!Physics::AddForceToBody(rigidbody->BodyHandle(), Force, bTreatAsAcceleration))
        {
            return false;
        }
    }
    return true;
}

template bool RocketManagerModule::ApplyForceToAllBodiesInGroup<ModuleAccessor<RigidbodyModule>>(const ModuleAccessor<RigidbodyModule>&,
      const RocketPartGroup&,
      const vec3&,
      const bool) const;

} // namespace ECSEngine
