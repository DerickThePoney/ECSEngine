#include "stdafx.h"

#include "SelectionManager.h"

#include "ApparenceModule.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "RenderingCore/GFXRepresentationProxy.h"

namespace ECSEngine
{
SelectionManager::SelectionManager()
    : ModuleSystem()
    , Singleton()
{
    RegisterDepency<ApparenceModule>(EEntityWorlds::BUILDINGS);
    WorldManager::Instance().RegisterListener(DELEGATE(&SelectionManager::OnUnitDeath, *this));
}

void SelectionManager::Finalize()
{
    ModuleSystem::Destroy();
    WorldManager::Instance().RemoveListener(DELEGATE(&SelectionManager::OnUnitDeath, *this));
}

void SelectionManager::Delete()
{
    Singleton<SelectionManager>::Destroy();
}

void SelectionManager::OnUnitDeath(const EntityId parUnit)
{
    FHighlightedUnits.erase(parUnit);
    FSelectedUnits.erase(parUnit);
}

void SelectionManager::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<ApparenceModule> apparenceAccessor(EEntityWorlds::BUILDINGS);

    FSelectedUnits.clear();
    FHighlightedUnits.clear();
    foreachitemconst(apparence, apparenceAccessor)
    {
        const Rendering::GFXRepresentationProxy* proxy = apparence.Proxy();
        auto selection = proxy->IsGFXSelectedOrHighlighted();
        if (selection.first)
            FSelectedUnits.insert(apparence.UnitId());
        if (selection.second)
            FHighlightedUnits.insert(apparence.UnitId());
    }
}
} // namespace ECSEngine