
#include "stdafx.h"

#include "RigidbodyModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "Physics/PhysicsAPI.h"

CEREAL_REGISTER_TYPE(ECSEngine::RigidbodyModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::RigidbodyModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(RigidbodyModule, RigidbodyModuleTemplate);

Module* RigidbodyModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<RigidbodyModule>(this, parUnitId, parParameters);
}

void RigidbodyModuleTemplate::VirtualDrawEditor()
{
    if (ImGui::CollapsingHeader("Body config"))
    {
        ImGui::Indent();
        FBodyConfig.DrawInEditor();
        ImGui::Unindent();
    }
}

IMPLEMENT_SAVELOAD_ABILITIES(RigidbodyModule);
template<typename Chunk, bool isWriting>
void RigidbodyModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
}

RigidbodyModule::RigidbodyModule()
    : Module()
{
}

RigidbodyModule::~RigidbodyModule()
{
}

void RigidbodyModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    mat4 worldMatrix = mat4::Identity();
    if (parParameters.HasParameter<ModuleParameters::Position>())
    {
        vec3 Position = parParameters.Get<ModuleParameters::Position>();
        worldMatrix.SetColumn(3, vec4::MakeHomogeneousVec4(Position));
    }

    const RigidbodyModuleTemplate* temp = Template<RigidbodyModuleTemplate>();
    FHandle = Physics::CreateNewPhysicsBody(worldMatrix, temp->GetBodyConfig());
}

void RigidbodyModule::VirtualDeinit()
{
    parent_type::VirtualDeinit();

    Physics::DestroyPhysicsBody(FHandle);
}

} // namespace ECSEngine
