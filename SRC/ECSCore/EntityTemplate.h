#pragma once
#include "EntityModuleKey.h"
#include "ModuleTemplate.h"
#include "WorldIds.h"

namespace ECSEngine
{
class EntityTemplate
{
    friend class EntityTemplateManager;

public:
    template<typename T>
    const bool HasModule() const
    {
        return FKey.HasModule();
    }

    const bool HasModule(const u32 parModuleId) const { return FKey.HasModule(parModuleId); }

    template<typename T>
    void SetHasModule()
    {
        FKey.SetHasModule<T>();
    }

    const void SetHasModule(const u32 parModuleId) { return FKey.SetHasModule(parModuleId); }

    ~EntityTemplate() {}

    void Initialise();

    const Worlds::Type& GetWorldId() const { return FWorld; }

    const ModuleTemplate* GetModuleTemplate(const u32 parId) const
    {
        auto it = FModuleTemplates.find(parId);
        if (it == FModuleTemplates.end())
            return nullptr;
        return it->second;
    }

#ifdef PERFORM_SECURITY_CHECKS
    bool IsInitialised() const { return FHasBeenInit; }
#endif

private:
    EntityTemplate()
        : FWorld(Worlds::STANDARD)
#ifdef PERFORM_SECURITY_CHECKS
        , FHasBeenInit(false)
#endif
    {
    }

private:
    Worlds::Type FWorld;
    EntityModuleKey FKey;
    std::map<u32, ModuleTemplate*> FModuleTemplates;

#ifdef PERFORM_SECURITY_CHECKS
    bool FHasBeenInit;
#endif
};
} // namespace ECSEngine