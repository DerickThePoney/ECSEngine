#pragma once
#include "EntityModuleKey.h"

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

private:
    EntityTemplate() {}

private:
    EntityModuleKey FKey;
};
} // namespace ECSEngine