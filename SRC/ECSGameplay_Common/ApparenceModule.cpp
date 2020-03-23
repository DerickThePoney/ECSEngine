#include "stdafx.h"

#include "ApparenceModule.h"

#include "Common/Resource.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "ECSCore/PropertyDrawer.h"
#include "RenderingCore/MeshManager.h"

CEREAL_REGISTER_TYPE(ECSEngine::ApparenceModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::ApparenceModuleTemplate)
namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(ApparenceModule, ApparenceModuleTemplate);

Module* ApparenceModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<ApparenceModule>(this, parUnitId, parParameters);
}

void ApparenceModuleTemplate::VirtualDrawEditor()
{
    PROPERTY_STRING("MeshFile", FMeshFileName, true, "*.fbx.gen");
}

ApparenceModule::ApparenceModule()
    : Module()
//, FProgram()
{
}

void ApparenceModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);
    FMeshHandle = Rendering::MeshManager::Instance().CreateMesh(Resource(Template<ApparenceModuleTemplate>()->GetMeshFileName()));
    // FProgram = parParameters.Get<ModuleParameters::Material>();

    const ApparenceModuleTemplate* temp = Template<ApparenceModuleTemplate>();
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