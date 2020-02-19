#include "stdafx.h"

#include "ApparenceModule.h"

#include "ECSBase/ModuleParameters.h"

namespace ECSEngine
{

ApparenceModule::ApparenceModule()
    : Module()
//, FProgram()
{
}

void ApparenceModule::VirtualInit(const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parParameters);
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