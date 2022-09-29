#include "stdafx.h"

#include "OrientationModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::OrientationModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::OrientationModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(OrientationModule, OrientationModuleTemplate);

Module* OrientationModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<OrientationModule>(this, parUnitId, parParameters);
}

void OrientationModuleTemplate::VirtualDrawEditor()
{
}

void OrientationModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    if (parParameters.HasParameter<ModuleParameters::EulerAnglesXYZ>())
    {
        const vec3 euler = parParameters.Get_IFP<ModuleParameters::EulerAnglesXYZ>(vec3(0.0f));
        FOrientation = quat::FromMat4(EulerAnglesXYZ(euler.x, euler.y, euler.z));
    }
    else if (parParameters.HasParameter<ModuleParameters::Orientation>())
    {
        FOrientation = parParameters.Get_IFP<ModuleParameters::Orientation>(quat());
    }
}

const vec3 OrientationModule::GetOrientationAsYawPitchRoll() const
{
    return ExtractEulerAnglesXYZ((mat4) FOrientation);
}

const vec3 OrientationModule::Forward() const
{
    mat4 rotationMatrix(FOrientation);
    return vec3(rotationMatrix.Column(0).xy0());
}

const vec3 OrientationModule::Right() const
{
    mat4 rotationMatrix(FOrientation);
    return vec3(rotationMatrix.Column(1).xy0());
}

const vec3 OrientationModule::Up() const
{
    mat4 rotationMatrix(FOrientation);
    return vec3(rotationMatrix.Column(2).xy0());
}

IMPLEMENT_SAVELOAD_ABILITIES(OrientationModule);
template<typename Chunk, bool isWriting>
void OrientationModule::SaveLoad(Chunk& parChunk)
{
    Module::SaveLoad(parChunk);

    parChunk& FOrientation;
}

} // namespace ECSEngine
