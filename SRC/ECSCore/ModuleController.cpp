#include "stdafx.h"

#include "ModuleController.h"

#include "Common/SavingSystemImplementation.h"

namespace ECSEngine
{
IMPLEMENT_VIRTUAL_SAVELOAD_ABILITIES(IModuleController);
template<typename Chunk, bool isWriting>
void IModuleController::SaveLoad(Chunk& parChunk)
{
    std::set<EntityId>& allocatedIds = GetAllocatedEntities();

    u32 sizeAllocated = allocatedIds.size();
    parChunk& sizeAllocated;

    if (isWriting)
    {
        std::set<EntityId>::iterator it = allocatedIds.begin();
        for (; it != allocatedIds.end(); ++it)
        {
            EntityId id = *it;
            parChunk& id;
            Module* mod = GetModulePtrForEntity(id);
            AssertRelease(mod != nullptr);
            parChunk& mod;
        }
    }
    else
    {
        forrange(i, 0, sizeAllocated)
        {
            EntityId unit;
            parChunk& unit;

#ifdef ENABLE_SECURITY_CHECKS
            {
                Module* mod = GetModulePtrForEntity(unit);
                AssertRelease(mod == nullptr);
            }
#endif
            AllocateForEntity(unit);

            Module* mod = GetModulePtrForEntity(unit);
            AssertRelease(mod != nullptr);
            parChunk& mod;

            allocatedIds.insert(unit);
        }
    }
}

} // namespace ECSEngine