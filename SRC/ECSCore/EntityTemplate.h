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
    ~EntityTemplate();

    template<typename T>
    const bool HasModule() const
    {
        return FKey.HasModule<T>();
    }

    const bool HasModule(const u32 parModuleId) const { return FKey.HasModule(parModuleId); }

    template<typename Module>
    void SetHasModule()
    {
        FKey.SetHasModule<Module>();
        AddModule(ModuleTraits<Module>::GetModuleId());
    }

    const void SetHasModule(const u32 parModuleId)
    {
        FKey.SetHasModule(parModuleId);
        AddModule(parModuleId);
    }

    void Initialise();

    void AddModule(const u32 parId);

    const Worlds::Type& GetWorldId() const { return FWorld; }

    template<typename Module>
    const ModuleTemplate* GetModuleTemplate() const
    {
        return GetModuleTemplate(ModuleTraits<Module>::GetModuleId());
    }

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
    void save(Archive& ar) const
    {
        ar(PROPERTY(Name), PROPERTY(World), PROPERTY(Key), NAMEDPROPERTY("ModuleTemplatesList", FModuleTemplates));
    }

    template<class Archive>
    void load(Archive& ar)
    {
        ar(PROPERTY(Name), PROPERTY(World), PROPERTY(Key), NAMEDPROPERTY("ModuleTemplatesList", FModuleTemplates));

        EntityModuleKey key;
        std::map<u32, std::unique_ptr<ModuleTemplate>> moduleTemplates;
        foreachitem(modTemplate, FModuleTemplates)
        {
            const u32 moduleId = modTemplate.second->GetModuleId();
            key.SetHasModule(moduleId);
            moduleTemplates.insert_or_assign(moduleId, std::move(modTemplate.second));
        }

        FKey = key;
        FModuleTemplates = std::move(moduleTemplates);
    }

    void DrawEditor();

private:
    template<typename T>
    void RemoveModule()
    {
        FKey.RemoveModule<T>();
    }

    const void RemoveModule(const u32 parModuleId) { return FKey.RemoveModule(parModuleId); }

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