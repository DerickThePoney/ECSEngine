
#include "stdafx.h"

#include "OrbitalPathFollowerModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::OrbitalPathFollowerModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::OrbitalPathFollowerModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(OrbitalPathFollowerModule, OrbitalPathFollowerModuleTemplate);

Module* OrbitalPathFollowerModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<OrbitalPathFollowerModule>(this, parUnitId, parParameters);
}

void OrbitalPathFollowerModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Orbit radius", FOrbitRadius);
    if (FOrbitRadius < 1.f)
        FOrbitRadius = 1.f;

    EDITOR_PROPERTY_ANGLE("Orbit phase", FOrbitPhase, -180.f, 180.f);
}

IMPLEMENT_SAVELOAD_ABILITIES(OrbitalPathFollowerModule);
template<typename Chunk, bool isWriting>
void OrbitalPathFollowerModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
    parChunk & FParentObject;
    parChunk & FCurrentTime;
}

OrbitalPathFollowerModule::OrbitalPathFollowerModule()
    : Module()
{
}

OrbitalPathFollowerModule::~OrbitalPathFollowerModule()
{
}

const float OrbitalPathFollowerModule::OrbitRadius() const
{
    const OrbitalPathFollowerModuleTemplate* Temp = Template<OrbitalPathFollowerModuleTemplate>();
    return Temp->OrbitRadius();
}

const float OrbitalPathFollowerModule::OrbitPhase() const
{
    const OrbitalPathFollowerModuleTemplate* Temp = Template<OrbitalPathFollowerModuleTemplate>();
    return Temp->OrbitPhase();
}

void OrbitalPathFollowerModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    FParentObject = parParameters.Get_IFP<ModuleParameters::OwnerId>(EntityId{ });
}

} // namespace ECSEngine
