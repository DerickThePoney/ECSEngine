#include "stdafx.h"

#include "ApparenceModule.h"

#include "Application/PropertyDrawer.h"
#include "Common/Resource.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "RenderingCore/MaterialManager.h"
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
    EDITOR_PROPERTY_STRING("MeshFile", FMeshFileName, true, "*.fbx.gen");
}

ApparenceModule::ApparenceModule()
    : Module()
{
}

void ApparenceModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);
    FMeshHandle = Rendering::MeshManager::Instance().CreateMesh(Resource(Template<ApparenceModuleTemplate>()->GetMeshFileName()));
    FMaterialHandle = Rendering::MaterialManager::CreateMaterialInstanceIFN(Template<ApparenceModuleTemplate>()->GetMaterialFileName());

    AssertRelease(FMeshHandle.IsValid());
    AssertRelease(FMaterialHandle.IsValid());

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

const Rendering::MaterialInstanceHandle& ApparenceModule::GetMaterialHandle() const
{
    return FMaterialHandle;
}

} // namespace ECSEngine