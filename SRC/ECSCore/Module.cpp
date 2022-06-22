#include "stdafx.h"

#include "Module.h"

#include "Common/SavingSystemImplementation.h"
#include "EntityTemplate.h"
#include "EntityTemplateManager.h"
#include "ModuleTemplate.h"

namespace ECSEngine
{

Module::Module()
{
}

void Module::Init(const ModuleTemplate* parTemplate, const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitCalled = false;
#endif

    FTemplate = parTemplate;

    VirtualInit(parUnitId, parParameters);

#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssertMsg(FVirtualInitCalled, "You forgot to call the parent's VirtualInit, you naughtyboy !");
#endif
}

void Module::Deinit()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDeinitCalled = false;
#endif

    VirtualDeinit();

#ifdef PERFORM_SECURITY_CHECKS
    AlwaysCheckedAssertMsg(FVirtualDeinitCalled, "You forgot to call the parent's VirtualDeinit, you naughtyboy !");
#endif
}

void Module::OnLoaded()
{
#ifdef ENABLE_SECURITY_CHECKS
    FVirtualOnLoadedCalled = false;
#endif

#ifdef ENABLE_SECURITY_CHECKS
    AlwaysCheckedAssertMsg(FVirtualOnLoadedCalled, "You forgot to call the parent's VirtualOnLoaded, you naughtyboy !");
#endif
}

void Module::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    FUnitId = parUnitId;
    AssertRelease(FUnitId.Valid());

#ifdef PERFORM_SECURITY_CHECKS
    FVirtualInitCalled = true;
#endif
}

void Module::VirtualDeinit()
{
#ifdef PERFORM_SECURITY_CHECKS
    FVirtualDeinitCalled = true;
#endif
}

void Module::VirtualOnLoaded()
{
#ifdef ENABLE_SECURITY_CHECKS
    FVirtualOnLoadedCalled = true;
#endif
}

IMPLEMENT_VIRTUAL_SAVELOAD_ABILITIES(Module);
template<typename Chunk, bool isWriting>
void Module::SaveLoad(Chunk& parChunk)
{
    parChunk& FUnitId;

    if (isWriting)
    {
        if (FTemplate != nullptr)
        {
            std::string templateName = FTemplate->GetTemplate()->GetName();
            parChunk& templateName;
        }
    }
    else
    {
        std::string templateName;
        parChunk& templateName;

        const EntityTemplate* entityTemplate = EntityTemplateManager::Instance().GetEntityTemplate(templateName);
        AssertRelease(entityTemplate != nullptr);
        FTemplate = entityTemplate->GetModuleTemplate(GetModuleId());
        AssertRelease(FTemplate != nullptr);
    }
}

} // namespace ECSEngine
