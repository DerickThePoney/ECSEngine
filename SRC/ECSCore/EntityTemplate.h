#pragma once
#include "EntityModuleKey.h"
#include "ModuleTemplate.h"
#include "WorldIds.h"

namespace ECSEngine
{
class EntityTemplate
{
public:
    EntityTemplate()
        : FWorld(Worlds::STANDARD)
        , FName("Default")
#ifdef PERFORM_SECURITY_CHECKS
        , FHasBeenInit(false)
#endif
    {
    }

    EntityTemplate(const EntityTemplate& other) = delete;
    EntityTemplate& operator=(const EntityTemplate& other) = delete;
    ~EntityTemplate() {}

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

    void Initialise();

    const Worlds::Type& GetWorldId() const { return FWorld; }

    const ModuleTemplate* GetModuleTemplate(const u32 parId) const
    {
        auto it = FModuleTemplates.find(parId);
        if (it == FModuleTemplates.end())
            return nullptr;
        return it->second.get();
    }

    const std::string& GetName() const { return FName; }

#ifdef PERFORM_SECURITY_CHECKS
    bool IsInitialised() const { return FHasBeenInit; }
#endif

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(Name), PROPERTY(World), PROPERTY(Key), NAMEDPROPERTY("ModuleTemplatesList", FModuleTemplates));
    }

private:
    Worlds::Type FWorld;
    EntityModuleKey FKey;
    std::map<u32, std::unique_ptr<ModuleTemplate>> FModuleTemplates;

    std::string FName;
#ifdef PERFORM_SECURITY_CHECKS
    bool FHasBeenInit;
#endif
};
} // namespace ECSEngine