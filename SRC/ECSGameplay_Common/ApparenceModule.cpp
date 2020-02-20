#include "stdafx.h"

#include "ApparenceModule.h"

#include "ECSCore/ModuleParameters.h"

namespace ECSEngine
{

ApparenceModule::ApparenceModule()
    : Module()
//, FProgram()
{
}

void ApparenceModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);
    FMeshHandle = parParameters.Get<ModuleParameters::Mesh>();
    // FProgram = parParameters.Get<ModuleParameters::Material>();
}

void ApparenceModule::VirtualDeinit()
{
    parent_type::VirtualDeinit();
}

const Rendering::MeshHandle& ApparenceModule::GetMeshHandle() const
{
    return FMeshHandle;
}

} // namespace ECSEngine